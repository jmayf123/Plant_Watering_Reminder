from django.shortcuts import render
from django.http import JsonResponse
from django.views.decorators.csrf import csrf_exempt

# Variable to store the latest moisture data
latest_moisture = None

@csrf_exempt  # This decorator should be directly above the function
def index(request):
    global latest_moisture

    if request.method == "POST":
        # Parse incoming data from the Arduino
        data = request.POST.get('sensor_value')
        print(f"Received sensor data: {data}")
        latest_moisture = data
        # Return a JSON response (you can modify as needed)
        return JsonResponse({'status': 'success', 'Val': latest_moisture})

    # Render the HTML page with the latest moisture data
    return render(request, 'index.html', {'Val': latest_moisture})


def get_moisture_data(request):
    global latest_moisture
    return JsonResponse({'Val': latest_moisture})