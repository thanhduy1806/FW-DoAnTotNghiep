# CI/CD Pipeline

The included CI skeleton shows the expected validation flow.

## Suggested stages

1. checkout
2. install Python dependencies
3. build firmware
4. run Robot smoke tests or prepare simulation validation
5. archive artifacts

## Recommended later enhancement

When Renode is available in your CI environment, extend the pipeline to run the simulation before the Robot stage.
