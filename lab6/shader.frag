#version 330 core

struct Ray {
    vec3 origin;
    vec3 direction;
};

struct Sphere {
    vec3 center;
    float radius;
    vec3 color;
    float reflectivity;
};

struct Plane {
    vec3 point;
    vec3 normal;
    vec3 color;
    float reflectivity;
};

struct Light {
    vec3 position;
    vec3 color;
};

uniform Sphere spheres[3]; // Передача сфер в шейдер
uniform Plane plane; // Передача плоскости в шейдер
uniform Light light; // Передача источника света в шейдер
uniform vec3 cameraPos; // Позиция камеры
uniform mat4 view; // Матрица вида
uniform int maxDepth;
uniform int width;
uniform int height;
uniform float teleportDistance; // Расстояние для телепортации
uniform vec3 initialCameraPos; // Начальная позиция камеры

out vec4 FragColor;

bool intersectSphere(const Ray ray, const Sphere sphere, out float t) {
    vec3 oc = ray.origin - sphere.center;
    float a = dot(ray.direction, ray.direction);
    float b = 2.0 * dot(oc, ray.direction);
    float c = dot(oc, oc) - sphere.radius * sphere.radius;
    float discriminant = b * b - 4 * a * c;

    if (discriminant < 0.0) return false;
    t = (-b - sqrt(discriminant)) / (2.0 * a);
    return t > 0.0;
}

bool intersectPlane(const Ray ray, const Plane plane, out float t) {
    float denom = dot(plane.normal, ray.direction);
    if (abs(denom) > 1e-6) {
        t = dot(plane.point - ray.origin, plane.normal) / denom;
        return t > 0.0;
    }
    return false;
}

vec3 trace(Ray ray) {
    vec3 finalColor = vec3(0.0);
    vec3 attenuation = vec3(1.0);

    for (int depth = 0; depth < (maxDepth > 0 ? maxDepth : 1); ++depth) {
        float tSphere = 1e20;
        float tPlane = 1e20;
        Sphere hitSphere;
        bool sphereHit = false;
        bool teleport = false;

        // Проверка пересечения со сферами
        for (int i = 0; i < 3; ++i) {
            float t;
            if (intersectSphere(ray, spheres[i], t) && t < tSphere) {
                tSphere = t;
                hitSphere = spheres[i];
                sphereHit = true;
                // Проверка на близость к третьему шару (зеленому)
                if (i == 2 && length(ray.origin - spheres[2].center) < teleportDistance) {
                    teleport = true;
                }
            }
        }

        // Проверка пересечения с плоскостью
        float t;
        if (intersectPlane(ray, plane, t)) {
            tPlane = t;
        }

        if (!sphereHit && tPlane == 1e20) {
            finalColor += attenuation * vec3(0.1); // Цвет фона
            break;
        }

        vec3 hitColor;
        vec3 hitPoint;
        vec3 normal;
        float reflectivity;

        if (tSphere < tPlane) {
            hitPoint = ray.origin + ray.direction * tSphere;
            normal = normalize(hitPoint - hitSphere.center);
            hitColor = hitSphere.color;
            reflectivity = hitSphere.reflectivity;
            if (teleport) {
                // Телепортация на начальную позицию камеры
                hitPoint = initialCameraPos;
            }
        } else {
            hitPoint = ray.origin + ray.direction * tPlane;
            normal = plane.normal;
            hitColor = plane.color;
            reflectivity = plane.reflectivity;
        }

        vec3 lightDir = normalize(light.position - hitPoint);
        float diffuse = max(dot(normal, lightDir), 0.0);

        vec3 reflectDir = reflect(-lightDir, normal);
        vec3 viewDir = normalize(-ray.direction);
        float specular = pow(max(dot(viewDir, reflectDir), 0.0), 16);

        vec3 ambient = light.color * 0.1; // Используем цвет света
        vec3 color = ambient + (diffuse + specular) * hitColor;

        finalColor += attenuation * color;
        attenuation *= reflectivity;

        // Подготовка к следующему отражению
        ray.origin = hitPoint + normal * 1e-4;
        ray.direction = reflect(ray.direction, normal);
    }

    return clamp(finalColor, 0.0, 1.0);
}

void main() {

    vec2 uv = gl_FragCoord.xy / vec2(width, height) * 2.0 - 1.0;
    vec3 direction = normalize(vec3(uv, -1.0));

    // Преобразование направления с учетом матрицы вида
    direction = direction * mat3(view);

    Ray ray;
    ray.origin = cameraPos;
    ray.direction = direction;

    vec3 color = trace(ray);
    FragColor = vec4(color, 1.0);
}