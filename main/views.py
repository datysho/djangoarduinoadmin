from constance import config
from django.http import JsonResponse, Http404


def get_config_view(request, constance_name):
    if hasattr(config, constance_name):
        return JsonResponse({constance_name: getattr(config, constance_name)})
    return Http404("Constance doesn't exist")
