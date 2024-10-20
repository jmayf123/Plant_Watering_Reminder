from django.shortcuts import render
from django.http import JsonResponse
from django.views.decorators.csrf import csrf_exempt

@csrf_exempt 

def index(request):
    if request.method == "POST":
        # Parse incoming data from the Arduino
        data = request.POST.get('sensor_value', 'No data')
        print(f"Received sensor data: {data}")
        return render(request, 'index.html', {'Val': data+'%'})
    else:
        return render(request, 'index.html', {'Val':'NA'})


