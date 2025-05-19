# <cj>GDNeko</c>
This [Geode](https://geode-sdk.org) mod brings the ancient desktop cat into <cg>Geometry Dash</c>!

<cb>The textures of Neko have been updated. If you want its classic look, get a texture pack from the releases!</c>

This mod is licensed under LGPL-2.1

## For mod developers
If you're a mod developer, you can add Neko to your menus without making this mod a dependency!

To do this, there are two events:

`geode::DispatchEvent<CCNode*>("timestepyt.gdneko/create-neko", this).post();`: If you want the bounds to match your node/layer.

`geode::DispatchEvent<CCNode*, CCRect>("timestepyt.gdneko/create-neko-rect", this, rect).post();`: The CCRect's position specifies the offset and the size specifies the size of the bounds.

Put the line in your layer/node's init method!