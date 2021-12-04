# EDA proyecto
 
## Build and compile RTree datastructure
```bash
mkdir build config
cd build
cmake ..
make
ctest
```

## To run the demo
```bash
cd data
wget # copy 1 or sveral links from source.txt and paste it here to download 
cd ../config
touch config.json
```
paste the following in config.json
```json
{
    "coordNames": [
        "Pickup_longitude",
        "Pickup_latitude"
    ],
    "files": [
        "paste here the absolute paths to the files you downloaded previously"
    ],
    "max": 1000000
}
```
