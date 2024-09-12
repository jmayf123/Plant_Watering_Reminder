from django.urls import path
from .views import show_data  # Import the correct view

urlpatterns = [
    path('show/', show_data, name='show_data'),  # Update the path to the new view
]