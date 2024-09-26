from django.shortcuts import render
from django.http import HttpResponse
# ESP8266_IP = 'http://192.168.4.1'

def index(request):
    # request.get() #Read the ESP8266
    return render(request, 'index.html', {'Val':'82%'})