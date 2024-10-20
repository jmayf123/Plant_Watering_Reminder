from django.urls import path
from .views import index, get_moisture_data

urlpatterns = [
    path('', index, name='index'),  # Main page
    path('get-moisture/', get_moisture_data, name='get_moisture'),  # New endpoint for GET requests
]