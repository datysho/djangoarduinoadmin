from django.urls import path

from . import views

urlpatterns = [
    path('config/<slug:constance_name>/', views.get_config_view),
]
