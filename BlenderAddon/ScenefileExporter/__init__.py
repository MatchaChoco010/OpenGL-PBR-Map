bl_info = {
    "name": "scenefile exporter",
    "author": "MatchaChoco010",
    "version": (0, 0, 1),
    "blender": (2, 82, 0),
    "location": "File > Export > Scenefile (.scenefile)",
    "description": "export scenefile!",
    "warning": "",
    "support": "TESTING",
    "wiki_url": "",
    "tracker_url": "",
    "category": "Import-Export"
}

if "bpy" in locals():
    import imp
    imp.reload(scenefile_exporter)
else:
    from . import scenefile_exporter

import bpy

def menu_fn(self, context):
    self.layout.separator()
    self.layout.operator(scenefile_exporter.ScenefileExporter.bl_idname)


def register():
    bpy.utils.register_class(scenefile_exporter.ScenefileExporter)
    bpy.types.TOPBAR_MT_file_export.append(menu_fn)


def unregister():
    bpy.types.TOPBAR_MT_file_export.remove(menu_fn)
    bpy.utils.unregister_class(scenefile_exporter.ScenefileExporter)


if __name__ == "__main__":
    register()
