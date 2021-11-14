# driver

Driver for z-synth control board. Receive parameter values and forward them over midi to z-synth.

```
# Install deps
python -m venv venv
source venv/bin/activate

# Calibrate adc/pot range
calibration out.json

# Create config file (paste in results from calibration)

# Development run
python -m driver.main --config example_config.json

# Install
python setup.py install
driver --mapping-config example_config.json
```
