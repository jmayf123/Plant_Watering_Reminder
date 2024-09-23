from django.urls import path
from . import views  # Import the correct view

urlpatterns = [
    path('', views.index, name='index')
]