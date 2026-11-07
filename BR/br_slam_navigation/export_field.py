import bpy
import os

BASE = "/home/luongngoctu/Downloads/Robocon2027/BR/br_slam_navigation"

BLEND = os.path.join(BASE, "map", "Robocon2027.blend")
GLB = os.path.join(
    BASE,
    "models",
    "robocon2027_field",
    "meshes",
    "Robocon2027.glb"
)

bpy.ops.wm.open_mainfile(filepath=BLEND)

# Bỏ Camera/Light, chỉ lấy mesh
bpy.ops.object.select_all(action='DESELECT')

mesh_objects = []

for obj in bpy.context.scene.objects:
    if obj.type == 'MESH':
        obj.select_set(True)
        mesh_objects.append(obj)

if not mesh_objects:
    raise RuntimeError("Khong tim thay mesh!")

bpy.context.view_layer.objects.active = mesh_objects[0]

print("\n========== OBJECTS EXPORT ==========")

for obj in mesh_objects:
    print(
        obj.name,
        "| location=",
        tuple(round(x, 4) for x in obj.location),
        "| dimensions=",
        tuple(round(x, 4) for x in obj.dimensions)
    )

print("\n========== MATERIALS ==========")

for obj in mesh_objects:
    for mat in obj.data.materials:
        if mat:
            print(obj.name, "->", mat.name)

os.makedirs(os.path.dirname(GLB), exist_ok=True)

print("\n========== EXPORT GLB ==========")

bpy.ops.export_scene.gltf(
    filepath=GLB,
    export_format='GLB',
    use_selection=True,
    export_materials='EXPORT',
    export_image_format='AUTO',
    export_apply=False
)

print("\n================================")
print("GLB EXPORT SUCCESS")
print(GLB)
print("================================")
