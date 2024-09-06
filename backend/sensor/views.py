from django.http import JsonResponse
from django.views.decorators.csrf import csrf_exempt
from django.shortcuts import render
from .models import SensorData  # Assuming you have a SensorData model

def show_data(request):
    data = SensorData.objects.all().order_by('-timestamp')  # Order by most recent
    return render(request, 'show_data.html', {'data': data})

@csrf_exempt  # Disable CSRF protection for simplicity; consider adding authentication for security
def receive_data(request):
    if request.method == 'POST':
        sensor_data = request.POST.get('sensorData', None)
        if sensor_data:
            # Save the data to your database
            SensorData.objects.create(data=sensor_data)
            return JsonResponse({'status': 'success', 'message': 'Data received'})
        else:
            return JsonResponse({'status': 'error', 'message': 'No data received'})
    return JsonResponse({'status': 'error', 'message': 'Invalid request method'}, status=405)
