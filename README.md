# NumSrv

[visit](https://ibanic.github.io/numsrv/)

Database that extrapolates and fills missing data. Retrieve any data you need.

## Module Compilation
```sh
cd numsrv/module
python3 setup.py bdist_wheel
```

## Module installation for server and for data importing scripts
```sh
cd numsrv/module
sudo apt-get install python3-venv
python3 -m venv ./numsrvenv
source numsrvenv/bin/activate
pip install wheel flask flask_cors
# .whl file should be inside dist dir
pip install /path/to/numsrvmodule.whl
```

## Ubuntu server installation

Prepare directory structure. Example:
```
/var/www/numsrv
    - web-server
        - app.py
        - converters.py
    - NumSrvHelper-0.1-cp38-cp38-linux_x86_64.whl
    - numsrvenv
    - data-compiled
        - 3
            - dim-keys-geo ...
            - ...
````

Follow tutorial on: [digitalocean.com](https://www.digitalocean.com/community/tutorials/how-to-serve-flask-applications-with-gunicorn-and-nginx-on-ubuntu-20-04)

## License
MIT
