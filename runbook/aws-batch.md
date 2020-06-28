# AWS Batch

## Build Docker image
Agents of AWS Batch use a Docker image named `manarimo` in ECR. Its build script is maintained in [batch-image](../batch-image) directory.
To update the image, follow instructions below:

1. Copy GitHub deploy key as `batch-image/deploy_key.pem`
2. Run commands
```
# Login to ECR
$ aws ecr get-login-password --region ap-northeast-1 | docker login --username AWS --password-stdin 806196602578.dkr.ecr.ap-northeast-1.amazonaws.com

# Build image
$ docker build -t 806196602578.dkr.ecr.ap-northeast-1.amazonaws.com/manarimo:latest

# Push image
$ docker push 806196602578.dkr.ecr.ap-northeast-1.amazonaws.com/manarimo:latest
```

## Execution steps
The `manarimo` docker image is supposed to execute following steps:

1. Clone `osak/ICFPC2020` repo into `/workspace`. It performs shallow clone - only the specified commit is downloaded.
2. Change current directory to where passed in `WORK_DIR` environment variable (relative to repository root).
3. Run a command passed in `RUN_COMMAND` environment variable.
4. (TODO) Copy the output to S3.