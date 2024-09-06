# Create your models here.
from django.db import models

class SensorData(models.Model):
    data = models.CharField(max_length=255)  # Store the sensor data as a string
    timestamp = models.DateTimeField(auto_now_add=True)  # Automatically add the current timestamp

    def __str__(self):
        return f"{self.data} at {self.timestamp}"
