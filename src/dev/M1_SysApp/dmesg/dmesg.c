/************************************************
 *  @file     : dmesg.c
 *  @date     : January 6, 2026
 *  @author   : CAO HIEU
 *  @version  : 2.0.0
 *-----------------------------------------------
 *  Description :
 ************************************************/

#include "dmesg.h"
#include "M5_Utils/Tick/tick.h"
#include "board.h"

#include "stdio.h"
#include "string.h"

#if defined(__ARM_ARCH_7M__) || defined(__ARM_ARCH_7EM__)
    // #define DMESG_DMB() __DMB()
    // #define DMESG_DSB() __DSB()
    #define DMESG_DMB()
    #define DMESG_DSB() 
#else
    #define DMESG_DMB() 
    #define DMESG_DSB() 
#endif

/* Entry structure with timestamp */
typedef struct __attribute__((packed)) {
    uint32_t timestamp;
    uint8_t length;
} DmesgEntry_Header_t;

#define DMESG_ENTRY_HEADER_SIZE sizeof(DmesgEntry_Header_t)
#define DMESG_ENTRY_MAX_SIZE (DMESG_ENTRY_HEADER_SIZE + DMESG_MSG_MAX_LENGTH)

/* Static buffer */
static uint8_t dmesg_buffer[DMESG_BUFFER_SIZE];

/* Buffer management - volatile for memory ordering */
static volatile size_t write_offset = 0;
static volatile size_t read_offset = 0;
static volatile size_t log_count = 0;
static volatile bool initialized = false;

/* Statistics */
static volatile uint32_t total_writes = 0;
static volatile uint32_t dropped_messages = 0;
static volatile uint32_t max_entries_reached = 0;

/* Corruption detection */
#define DMESG_MAGIC 0xDEADBEEFU
static volatile uint32_t dmesg_magic = DMESG_MAGIC;

/*-------------------------------------------
 * Private Functions
 *-------------------------------------------*/

static inline bool is_buffer_corrupted(void) {
    return (dmesg_magic != DMESG_MAGIC);
}

static inline size_t get_available_space(void) {
    size_t w_off = write_offset;
    size_t r_off = read_offset;
    
    if (r_off <= w_off) {
        return DMESG_BUFFER_SIZE - (w_off - r_off) - 1U;
    } else {
        return (r_off - w_off) - 1U;
    }
}

static void evict_oldest_entry(void) {
    if (log_count == 0U) {
        return;
    }

    size_t r_off = read_offset;
    
    /* Validate read offset */
    if (r_off >= DMESG_BUFFER_SIZE) {
        read_offset = 0U;
        log_count = 0U;
        return;
    }

    /* Read header safely */
    DmesgEntry_Header_t header;
    for (size_t i = 0; i < DMESG_ENTRY_HEADER_SIZE; i++) {
        ((uint8_t*)&header)[i] = dmesg_buffer[r_off];
        r_off = (r_off + 1U) % DMESG_BUFFER_SIZE;
    }

    /* Validate length */
    if (header.length == 0U || header.length > DMESG_MSG_MAX_LENGTH) {
        /* Corrupted - reset buffer */
        read_offset = write_offset;
        log_count = 0U;
        dropped_messages++;
        return;
    }

    /* Advance read pointer */
    read_offset = (r_off + header.length) % DMESG_BUFFER_SIZE;
    log_count--;
}

static Dmesg_Error_t write_entry_internal(const char *msg, size_t len, uint32_t timestamp) {
    /* Input validation */
    if (msg == NULL) {
        return DMESG_ERROR_NULL_POINTER;
    }
    
    if (len == 0U || len > DMESG_MSG_MAX_LENGTH) {
        return DMESG_ERROR_INVALID_LENGTH;
    }

    /* Check corruption */
    if (is_buffer_corrupted()) {
        return DMESG_ERROR_CORRUPTED_ENTRY;
    }

    size_t required_space = DMESG_ENTRY_HEADER_SIZE + len;

    /* Make space by evicting old entries */
    size_t eviction_count = 0;
    while (get_available_space() < required_space && log_count > 0U) {
        evict_oldest_entry();
        eviction_count++;
        
        /* Safety: prevent infinite loop if buffer logic corrupted */
        if (eviction_count > log_count + 10U) {
            /* Full reset */
            write_offset = 0;
            read_offset = 0;
            log_count = 0;
            dropped_messages++;
            return DMESG_ERROR_BUFFER_FULL;
        }
    }
    
    if (eviction_count > 0U) {
        dropped_messages += eviction_count;
    }

    /* Final space check */
    if (get_available_space() < required_space) {
        return DMESG_ERROR_BUFFER_FULL;
    }

    /* Prepare header */
    DmesgEntry_Header_t header;
    header.timestamp = timestamp;
    header.length = (uint8_t)len;

    size_t w_off = write_offset;

    /* Write header */
    for (size_t i = 0; i < DMESG_ENTRY_HEADER_SIZE; i++) {
        dmesg_buffer[w_off] = ((uint8_t*)&header)[i];
        w_off = (w_off + 1U) % DMESG_BUFFER_SIZE;
    }

    /* Write message data */
    for (size_t i = 0; i < len; i++) {
        dmesg_buffer[w_off] = (uint8_t)msg[i];
        w_off = (w_off + 1U) % DMESG_BUFFER_SIZE;
    }

    /* Memory barrier before updating write pointer */
    DMESG_DSB();
    
    /* Commit write */
    write_offset = w_off;
    log_count++;
    total_writes++;
    
    /* Update max entries stat */
    if (log_count > max_entries_reached) {
        max_entries_reached = (uint32_t)log_count;
    }

    return DMESG_OK;
}

static Dmesg_Error_t read_entry_at_offset(size_t *offset, char *buffer, 
                                           size_t buffer_size, uint32_t *timestamp) {
    if (offset == NULL || buffer == NULL || timestamp == NULL) {
        return DMESG_ERROR_NULL_POINTER;
    }

    if (*offset >= DMESG_BUFFER_SIZE) {
        return DMESG_ERROR_CORRUPTED_ENTRY;
    }

    /* Read header */
    DmesgEntry_Header_t header;
    size_t off = *offset;
    
    for (size_t i = 0; i < DMESG_ENTRY_HEADER_SIZE; i++) {
        ((uint8_t*)&header)[i] = dmesg_buffer[off];
        off = (off + 1U) % DMESG_BUFFER_SIZE;
    }

    /* Validate length */
    if (header.length == 0U || header.length > DMESG_MSG_MAX_LENGTH) {
        return DMESG_ERROR_CORRUPTED_ENTRY;
    }

    *timestamp = header.timestamp;

    /* Read message with bounds checking */
    size_t copy_len = (header.length < buffer_size - 1U) ? 
                      header.length : (buffer_size - 1U);
    
    for (size_t i = 0; i < copy_len; i++) {
        buffer[i] = (char)dmesg_buffer[off];
        off = (off + 1U) % DMESG_BUFFER_SIZE;
    }
    
    /* Skip remaining bytes if truncated */
    for (size_t i = copy_len; i < header.length; i++) {
        off = (off + 1U) % DMESG_BUFFER_SIZE;
    }

    buffer[copy_len] = '\0';
    *offset = off;
    
    return DMESG_OK;
}

/*-------------------------------------------
 * Public API
 *-------------------------------------------*/

Dmesg_Error_t Dmesg_Init(void) {
    /* Prevent double init */
    if (initialized) {
        return DMESG_OK;
    }

    /* Reset all state */
    write_offset = 0;
    read_offset = 0;
    log_count = 0;
    total_writes = 0;
    dropped_messages = 0;
    max_entries_reached = 0;
    dmesg_magic = DMESG_MAGIC;

    /* Clear buffer */
    (void)memset(dmesg_buffer, 0, DMESG_BUFFER_SIZE);

    /* Synchronize */
    DMESG_DSB();
    initialized = true;
    
    return DMESG_OK;
}

bool Dmesg_IsInitialized(void) {
    DMESG_DMB();
    return initialized;
}

Dmesg_Error_t Dmesg_Write(const char *msg) {
    if (!initialized) {
        return DMESG_ERROR_NOT_INITIALIZED;
    }
    
    if (msg == NULL) {
        return DMESG_ERROR_NULL_POINTER;
    }

    /* Calculate length safely */
    size_t len = 0;
    const char *p = msg;
    while (*p != '\0' && len < DMESG_MSG_MAX_LENGTH) {
        len++;
        p++;
    }

    if (len == 0U) {
        return DMESG_ERROR_INVALID_LENGTH;
    }

    uint32_t timestamp = Utils_GetTick();
    
    /* Memory barrier before write */
    DMESG_DMB();
    
    return write_entry_internal(msg, len, timestamp);
}

Dmesg_Error_t Dmesg_WriteISR(const char *msg) {
 
    return Dmesg_Write(msg);
}

void Dmesg_GetLogs(void) {
    if (!initialized) {
        return;
    }

    DMESG_DMB();

    size_t offset = read_offset;
    size_t entries_to_read = log_count;
    
    if (entries_to_read == 0U) {
        DEBUG_SendString("[DMESG] No logs available\r\n");
        return;
    }

    char buffer[DMESG_MSG_MAX_LENGTH + 32];

    for (size_t i = 0; i < entries_to_read; i++) {
        char msg_buf[DMESG_MSG_MAX_LENGTH + 1];
        uint32_t timestamp = 0;

        Dmesg_Error_t err = read_entry_at_offset(&offset, msg_buf, 
                                                  sizeof(msg_buf), &timestamp);
        
        if (err == DMESG_OK) {
            int written = snprintf(buffer, sizeof(buffer), "[%10lu] %s\r\n", 
                                   (unsigned long)timestamp, msg_buf);
            if (written > 0 && written < (int)sizeof(buffer)) {
                DEBUG_SendString(buffer);
            }
        } else {
            DEBUG_SendString("[DMESG] Corrupted entry detected\r\n");
            break;
        }
    }
}

void Dmesg_GetLatestN(size_t N) {
    if (!initialized || N == 0U) {
        return;
    }

    DMESG_DMB();

    size_t available = log_count;
    if (N > available) {
        N = available;
    }
    
    if (N == 0U) {
        DEBUG_SendString("[DMESG] No logs available\r\n");
        return;
    }

    /* Calculate skip count */
    size_t skip = available - N;
    size_t offset = read_offset;

    /* Skip older entries */
    for (size_t i = 0; i < skip; i++) {
        if (offset >= DMESG_BUFFER_SIZE) {
            DEBUG_SendString("[DMESG] Buffer corruption detected\r\n");
            return;
        }
        
        DmesgEntry_Header_t header;
        size_t tmp_offset = offset;
        
        for (size_t j = 0; j < DMESG_ENTRY_HEADER_SIZE; j++) {
            ((uint8_t*)&header)[j] = dmesg_buffer[tmp_offset];
            tmp_offset = (tmp_offset + 1U) % DMESG_BUFFER_SIZE;
        }
        
        if (header.length == 0U || header.length > DMESG_MSG_MAX_LENGTH) {
            DEBUG_SendString("[DMESG] Entry corruption during skip\r\n");
            return;
        }
        
        offset = (tmp_offset + header.length) % DMESG_BUFFER_SIZE;
    }

    /* Print N entries */
    char buffer[DMESG_MSG_MAX_LENGTH + 32];
    
    for (size_t i = 0; i < N; i++) {
        char msg_buf[DMESG_MSG_MAX_LENGTH + 1];
        uint32_t timestamp = 0;

        Dmesg_Error_t err = read_entry_at_offset(&offset, msg_buf, 
                                                  sizeof(msg_buf), &timestamp);
        
        if (err == DMESG_OK) {
            int written = snprintf(buffer, sizeof(buffer), "[%10lu] %s\r\n",
                                   (unsigned long)timestamp, msg_buf);
            if (written > 0 && written < (int)sizeof(buffer)) {
                DEBUG_SendString(buffer);
            }
        } else {
            DEBUG_SendString("[DMESG] Entry corruption during read\r\n");
            break;
        }
    }
}

Dmesg_Error_t Dmesg_Clear(void) {
    if (!initialized) {
        return DMESG_ERROR_NOT_INITIALIZED;
    }

    /* Reset pointers */
    write_offset = 0;
    read_offset = 0;
    log_count = 0;
    
    DMESG_DSB();
    
    return DMESG_OK;
}

Dmesg_Stats_t Dmesg_GetStats(void) {
    Dmesg_Stats_t stats;
    (void)memset(&stats, 0, sizeof(stats));
    
    if (!initialized) {
        return stats;
    }

    DMESG_DMB();
    
    stats.total_writes = total_writes;
    stats.dropped_messages = dropped_messages;
    stats.current_entries = (uint32_t)log_count;
    stats.max_entries_reached = max_entries_reached;
    
    /* Calculate utilization */
    size_t available = get_available_space();
    size_t used = DMESG_BUFFER_SIZE - available - 1U;
    stats.buffer_utilization_percent = 
        (uint32_t)((used * 100U) / DMESG_BUFFER_SIZE);
    
    return stats;
}