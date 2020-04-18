import os
import shutil
import subprocess
import mathutils
import bmesh
import bpy
from bpy_extras.io_utils import ExportHelper


class ScenefileExporter(bpy.types.Operator, ExportHelper):
    # "Scene" と名付けられたCollection内部のオブジェクトを
    # scenefileとして書き出す

    bl_idname = "export_scene.scenefile_exporter"
    bl_label = "Scenefile Exporter"
    bl_optioins = {}

    filename_ext = ".scenefile"

    @classmethod
    def poll(cls, context):
        return "Scene" in bpy.data.collections

    def execute(self, context):
        pwd = os.getcwd()
        os.chdir(os.path.dirname(__file__))

        print(self.filepath)
        if os.path.exists(self.filepath):
            raise Exception("Already exists {0}".format(self.filepath))
        os.makedirs(self.filepath)

        scene_collection = bpy.data.collections['Scene']

        meshes_dict = {}
        materials_dict = {}
        mesh_entities = []
        directional_light = ""
        point_lights = []
        spot_lights = []
        sky = ""

        # Meshes
        for obj in scene_collection.all_objects:
            if not obj.type == "MESH":
                continue

            me = obj.data
            mesh_name = me.name

            # Add Mesh
            if not mesh_name in meshes_dict:
                me_copy = obj.to_mesh()
                bm = bmesh.new()
                bm.from_mesh(me_copy)
                bmesh.ops.triangulate(bm, faces=bm.faces)
                bm.to_mesh(me_copy)
                bm.free()

                me_copy.calc_normals_split()

                mesh_vertices_text = ""
                mesh_normals_text = ""
                mesh_uvs_text = ""

                for l in me_copy.loops:
                    co = me_copy.vertices[l.vertex_index].co
                    n = l.normal
                    uv = me_copy.uv_layers.active.data[l.index].uv
                    mesh_vertices_text += "v {0} {1} {2}\n" \
                        .format(co.x, co.y, co.z)
                    mesh_normals_text += "vn {0} {1} {2}\n" \
                        .format(n.x, n.y, n.z)
                    mesh_uvs_text += "vt {0} {1}\n"\
                        .format(uv.x, uv.y)

                mesh_text = "Mesh: " + mesh_name + "\n" +\
                    mesh_vertices_text + mesh_normals_text + mesh_uvs_text +\
                    "MeshEnd\n"
                meshes_dict[mesh_name] = mesh_text

                del bm, me_copy, mesh_vertices_text, mesh_normals_text, mesh_uvs_text, mesh_text

            material = obj.material_slots[0].material
            material_name = material.name

            # Add Material
            if not material_name in materials_dict:
                os.makedirs(
                    os.path.join(self.filepath, "Materials", material_name))

                # Material Output-Surface
                principledBSDF = material.node_tree.nodes[0].inputs[0].links[0].from_node

                # PrincipledBSDF-BaseColor
                if len(principledBSDF.inputs[0].links) == 0:
                    raise Exception(
                        "{0} - BaseColor Texture Node is missing.".format(material_name))
                # PrincipledBSDF-Metallic
                elif len(principledBSDF.inputs[4].links) == 0:
                    raise Exception(
                        "{0} - Metallic Texture Node is missing".format(material_name))
                # PrincipledBSDF-Roughness
                elif len(principledBSDF.inputs[7].links) == 0:
                    raise Exception(
                        "{0} - Roughness Texture Node is missing".format(material_name))
                # PrincipledBSDF-Emission
                elif len(principledBSDF.inputs[17].links) == 0:
                    raise Exception(
                        "{0} - Emission Node Node is missing".format(material_name))
                elif len(principledBSDF.inputs[17].links[0].from_node.inputs[0].links) == 0:
                    raise Exception(
                        "{0} - Emission Texture Node is missing".format(material_name))
                # PrincipledBSDF-Normal
                elif len(principledBSDF.inputs[19].links) == 0:
                    raise Exception(
                        "{0} - Normal Node is missing".format(material_name))
                elif len(principledBSDF.inputs[19].links[0].from_node.inputs[1].links) == 0:
                    raise Exception(
                        "{0} - Normal Texture Node is missing".format(material_name))

                baseColorImageNode = principledBSDF.inputs[0].links[0].from_node
                metallicImageNode = principledBSDF.inputs[4].links[0].from_node
                roughnessImageNode = principledBSDF.inputs[7].links[0].from_node
                emissionMultiplyNode = principledBSDF.inputs[17].links[0].from_node
                emissionImageNode = emissionMultiplyNode.inputs[0].links[0].from_node
                normalMapImageNode = principledBSDF.inputs[19].links[0].\
                    from_node.inputs[1].links[0].from_node

                if baseColorImageNode.image == None:
                    raise Exception(
                        "{0} - BaseColor Texture is None".format(material_name))
                elif metallicImageNode.image == None:
                    raise Exception(
                        "{0} - Metallic Texture is None".format(material_name))
                elif roughnessImageNode.image == None:
                    raise Exception(
                        "{0} - Roughness Texture is None".format(material_name))
                elif emissionImageNode.image == None:
                    raise Exception(
                        "{0} - Emission Texture is None".format(material_name))
                elif normalMapImageNode.image == None:
                    raise Exception(
                        "{0} - NormalMap Texture is None".format(material_name))

                baseColor_ext = os.path.splitext(
                    baseColorImageNode.image.filepath)[1]
                shutil.copy2(
                    bpy.path.abspath(baseColorImageNode.image.filepath),
                    os.path.join(self.filepath, "Materials", material_name,
                                 "baseColor" + baseColor_ext)
                )
                metallic_ext = os.path.splitext(
                    metallicImageNode.image.filepath)[1]
                shutil.copy2(
                    bpy.path.abspath(metallicImageNode.image.filepath),
                    os.path.join(self.filepath, "Materials", material_name,
                                 "metallic" + metallic_ext)
                )
                roughness_ext = os.path.splitext(
                    roughnessImageNode.image.filepath)[1]
                shutil.copy2(
                    bpy.path.abspath(roughnessImageNode.image.filepath),
                    os.path.join(self.filepath, "Materials", material_name,
                                 "roughness" + roughness_ext)
                )
                emission_ext = os.path.splitext(
                    emissionImageNode.image.filepath)[1]
                shutil.copy2(
                    bpy.path.abspath(emissionImageNode.image.filepath),
                    os.path.join(self.filepath, "Materials", material_name,
                                 "emission" + emission_ext)
                )
                normalMap_ext = os.path.splitext(
                    normalMapImageNode.image.filepath)[1]
                shutil.copy2(
                    bpy.path.abspath(normalMapImageNode.image.filepath),
                    os.path.join(self.filepath, "Materials", material_name,
                                 "normalMap" + normalMap_ext)
                )

                material_text = "Material: {0}\n".format(material_name)
                material_text += "baseColor: Materials/{0}/baseColor{1}\n"\
                    .format(material_name, baseColor_ext)
                material_text += "metallic: Materials/{0}/metallic{1}\n"\
                    .format(material_name, metallic_ext)
                material_text += "roughness: Materials/{0}/roughness{1}\n"\
                    .format(material_name, roughness_ext)
                material_text += "emission: Materials/{0}/emission{1}\n"\
                    .format(material_name, emission_ext)
                material_text += "normalMap: Materials/{0}/normalMap{1}\n"\
                    .format(material_name, normalMap_ext)
                material_text += "emissionIntensity: {0}\n".format(
                    emissionMultiplyNode.inputs[1].links[0].from_node.outputs[0].default_value * 683.002)
                material_text += "MaterialEnd\n"

                materials_dict[material_name] = material_text

            pos = obj.matrix_world.to_translation()
            rot = obj.matrix_world.to_euler('YXZ')
            scl = obj.matrix_world.to_scale()

            entity_text = "MeshEntity: {0}\n".format(obj.name)
            entity_text += "Mesh: {0}\n".format(mesh_name)
            entity_text += "Material: {0}\n".format(material_name)
            entity_text += "Position: {0} {1} {2}\n".format(
                pos.x, pos.y, pos.z)
            entity_text += "Rotation: {0} {1} {2} {3}\n".format(
                rot.x, rot.y, rot.z, rot.order)
            entity_text += "Scale: {0} {1} {2}\n".format(scl.x, scl.y, scl.z)
            entity_text += "MeshEntityEnd\n"

            mesh_entities.append(entity_text)

        # Lights
        for obj in scene_collection.all_objects:
            if not obj.type == "LIGHT":
                continue

            light = obj.data

            if light.type == "POINT":
                pos = obj.matrix_world.to_translation()

                # W to lumen at wavelength = 555nm (green)
                intensity = light.energy * 683.002

                color = light.color
                range = light.cutoff_distance
                clip_start = light.shadow_buffer_clip_start
                shadow_bias = light.shadow_buffer_bias
                use_shadow = int(light.use_shadow)

                point_light_text = "PointLight: {0}\n".format(obj.name)
                point_light_text += "Position: {0} {1} {2}\n".format(
                    pos.x, pos.y, pos.z)
                point_light_text += "Intensity: {0}\n".format(intensity)
                point_light_text += "Color: {0} {1} {2}\n".format(
                    color.r, color.g, color.b)
                point_light_text += "Range: {0}\n".format(range)
                point_light_text += "ClipStart: {0}\n".format(clip_start)
                point_light_text += "ShadowBias: {0}\n".format(shadow_bias)
                point_light_text += "UseShadow: {0}\n".format(use_shadow)
                point_light_text += "PointLightEnd\n"
                point_lights.append(point_light_text)

            if light.type == "SUN":
                pos = obj.matrix_world.to_translation()
                rot = obj.matrix_world.to_euler('YXZ')

                # W/m2 to lumen/m2 at wavelength = 555nm (green)
                intensity = light.energy * 683.002

                color = light.color
                dir = mathutils.Vector((0.0, 0.0, -1.0))
                dir.rotate(rot)

                directional_light += "DirectionalLight: {0}\n".format(obj.name)
                directional_light += "Position: {0} {1} {2}\n".format(
                    pos.x, pos.y, pos.z)
                directional_light += "Intensity: {0}\n".format(intensity)
                directional_light += "Color: {0} {1} {2}\n".format(
                    color.r, color.g, color.b)
                directional_light += "Direction: {0} {1} {2}\n".format(
                    dir.x, dir.y, dir.z)
                directional_light += "DirectionalLightEnd\n"

            if light.type == "SPOT":
                pos = obj.matrix_world.to_translation()
                rot = obj.matrix_world.to_euler('YXZ')

                # W to lumen at wavelength = 555nm (green)
                intensity = light.energy * 683.002

                color = light.color
                range = light.cutoff_distance
                clip_start = light.shadow_buffer_clip_start
                dir = mathutils.Vector((0.0, 0.0, -1.0))
                dir.rotate(rot)
                angle = light.spot_size
                blend = light.spot_blend

                spot_light_text = "SpotLight: {0}\n".format(obj.name)
                spot_light_text += "Position: {0} {1} {2}\n".format(
                    pos.x, pos.y, pos.z)
                spot_light_text += "Intensity: {0}\n".format(intensity)
                spot_light_text += "Color: {0} {1} {2}\n".format(
                    color.r, color.g, color.b)
                spot_light_text += "Direction: {0} {1} {2}\n".format(
                    dir.x, dir.y, dir.z)
                spot_light_text += "Range: {0}\n".format(range)
                spot_light_text += "ClipStart: {0}\n".format(clip_start)
                spot_light_text += "Angle: {0}\n".format(angle)
                spot_light_text += "Blend: {0}\n".format(blend)
                spot_light_text += "SpotLightEnd\n"
                spot_lights.append(spot_light_text)

        # Sky
        sky_image = bpy.data.worlds["World"].node_tree.nodes["Environment Texture"].image
        os.makedirs(os.path.join(self.filepath, "Sky"))
        sky_ext = os.path.splitext(sky_image.filepath)[1]
        if sky_ext != ".exr":
            raise Exception("Environment Texture must be .exr format.")
        shutil.copy2(
            bpy.path.abspath(sky_image.filepath),
            os.path.join(self.filepath, "Sky", "sky.exr")
        )
        sky_intensity = bpy.data.worlds["World"].node_tree.nodes["Background"].inputs[1].default_value * 683.002
        sky += "Sky:\n"
        sky += "SkyImagePath: Sky/sky.exr\n"
        sky += "skyIntensity: {0}\n".format(sky_intensity)
        sky += "SkyEnd\n"

        # Global Diffuse IBL
        os.makedirs(os.path.join(self.filepath, "GlobalIBL", "Diffuse"))
        srcpath = os.path.join(self.filepath, "Sky", "sky.exr")
        dstpath = os.path.join(self.filepath, "GlobalIBL", "Diffuse")
        subprocess.call(["IBL-Diffuse.exe", srcpath,
                         str(sky_intensity), dstpath])

        # Global Specular IBL
        os.makedirs(os.path.join(self.filepath, "GlobalIBL", "Specular"))
        srcpath = os.path.join(self.filepath, "Sky", "sky.exr")
        dstpath = os.path.join(self.filepath, "GlobalIBL", "Specular")
        subprocess.call(["IBL-Specular.exe", srcpath,
                         str(sky_intensity), dstpath])

        scene_text = "# Scene file\n"
        for mesh_text in meshes_dict.values():
            scene_text += mesh_text
        for material_text in materials_dict.values():
            scene_text += material_text
        for mesh_entity in mesh_entities:
            scene_text += mesh_entity
        scene_text += directional_light
        for point_light in point_lights:
            scene_text += point_light
        for spot_light in spot_lights:
            scene_text += spot_light
        scene_text += sky

        with open(os.path.join(self.filepath, "scenefile.txt"), mode="w") as f:
            f.write(scene_text)

        os.chdir(pwd)

        return {"FINISHED"}
