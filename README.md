# <cj>GDNeko</c>
This [Geode](https://geode-sdk.org) mod brings the ancient desktop cat into <cg>Geometry Dash</c>'s main menu!

<cb>The textures of Neko have been updated. If you want its classic look, get a texture pack.</c>

## For mod developers
If you're a mod developer, you can add Neko to your menus without making this mod a dependency!

To do this, simply add `geode::DispatchEvent<CCNode*>("timestepyt.gdneko/create-neko", this).post();` to your node/layer's init method if you want the bounds to match!
Otherwise, you can also do `geode::DispatchEvent<CCNode*, CCRect>("timestepyt.gdneko/create-neko-rect", this, rect).post();`, where the CCRect's position specifies the offset and the size specifies the size of the bounds!