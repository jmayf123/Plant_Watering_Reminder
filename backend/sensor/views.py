from django.shortcuts import render

# Create your views here.
from django.http import HttpResponse
from django.views.decorators.csrf import csrf_exempt

@csrf_exempt  # Disable CSRF protection for simplicity (optional)
def receive_data(request):
    if request.method == 'POST':
        sensor_data = request.POST.get('sensorData', None)
        if sensor_data:
            # Process the sensor data here (e.g., save to a database or log it)
            print(f"Received sensor data: {sensor_data}")
            return HttpResponse("Data received")
        else:
            return HttpResponse("No data received")
    else:
        return HttpResponse("Invalid request method", status=405)
