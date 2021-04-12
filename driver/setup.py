from setuptools import setup

setup(
    name='driver',
    version='0.0.0',
    packages=['driver'],
    entry_points={
        'console_scripts': [
            'driver=driver.main:main',
            'calibration=driver.calibration:main',
        ],
    }
)
