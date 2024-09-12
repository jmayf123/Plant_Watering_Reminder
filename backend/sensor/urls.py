from django.urls import path
from . import views  # Import the correct view

urlpatterns = [
    path('', views.show_data, name='show_data'),  # Update the path to the new view
]