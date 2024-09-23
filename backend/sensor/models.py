# Create your models here.
from django.db import models

class data(models.Model):
    
    time_stamp = models.DateTimeField(auto_now_add=True)  # Automatically add the current timestamp
    date = models.DateField(auto_now_add=True) 
    sensor_value = models.IntegerField()  # Store the sensor data as a int
    
