from dataclasses import dataclass
from typing import List, Dict, Tuple, overload, NewType
from pilotlight.pilotlight import plVec2, plVec3, plVec4, plMat4
from pilotlight.enums import *

########################################################################################################################
# [SECTION] opaque types
########################################################################################################################

plFont = NewType("plFont", object)
plCommandBuffer = NewType("plCommandBuffer", object)
plFontAtlas = NewType("plFontAtlas", object)
plDrawList2D = NewType("plDrawList2D", object)
plDrawList3D = NewType("plDrawList3D", object)
plDrawLayer2D = NewType("plDrawLayer2D", object)
plWindow = NewType("plWindow", object)
plDevice = NewType("plDevice", object)
plSwapchain = NewType("plSwapchain", object)
plComponentLibrary = NewType("plComponentLibrary", object)
plPakFile = NewType("plPakFile", object)
plDoublePointer = NewType("plDoublePointer", object)

# (ecs type key, entity index, entity generation)
type plEntity = Tuple[int, int, int]

# plAssetHandle.uData (64 bit integer)
type plAssetHandle = int

type plQuat = plVec4

class plRenderAttachmentInfo:
    def __init__(self):
        self.aeColorFormats = []
        self.eDepthFormat = plFormat.PL_FORMAT_UNKNOWN
        self.eStencilFormat = plFormat.PL_FORMAT_UNKNOWN

class plShaderMacroDefinition:
    def __init__(self):
        self.pcName = None
        self.pcValue = None

class plShaderOptions:
    def __init__(self):
        self.eFlags = 0
        self.eOptimizationLevel = 0
        self.ptMacroDefinitions = []
        self.apcIncludeDirectories = []
        self.apcDirectories = []

@dataclass(slots=True)
class plStarterInit:
    eFlags: plStarterFlag
    ptWindow: plWindow

@dataclass(slots=True)
class plDrawSolidOptions:
    uColor: int = 0xFFFFFFFF  # default white

@dataclass(slots=True)
class plDrawLineOptions:
    uColor: int = 0xFFFFFFFF  # default white
    fThickness: float = 1.0
    tFlags: int = 0 # plDrawStrokeFlag (2D only), default: centered stroke

@dataclass(slots=True)
class plDrawInit:
    ptDevice: plDevice | None = None

@dataclass(slots=True)
class plDrawTextOptions:
    ptFont: plFont | None = None
    fSize: float = 0.0
    uColor: int = 0xFFFFFFFF  # default white
    fWrap: float = 0.0
    # TODO: need to add transform once plMat3 is binded

@dataclass(slots=True)
class plFontRange:
    iFirstCodePoint: int = 0
    uCharCount: int = 0

class plFontConfig:
    fSize: float = 0.0
    ptRanges: List[plFontRange] = []
    piIndividualChars: List[int] = []
    ptMergeFont: plFont | None = None
    uVOverSampling: int = 0 # BITMAP ONLY
    uHOverSampling: int = 0 # BITMAP ONLY

class plResourceManagerInit:
    ptDevice: plDevice
    uMaxTextureResolution: int = 1024

@dataclass(slots=True)
class plWindowDesc:
    tMode: int = 0 # PL_WINDOW_PRESENTATION_MODE_GRAPHICS_API
    tFlags: int = 0
    pcTitle: str = "Untitled Window"
    uWidth: int = 500
    uHeight: int = 500
    iXPos: int = 200
    iYPos: int = 200

@dataclass(slots=True)
class plLogExtChannelInit:
    tType: int = 0         # plLogChannelType (0 = PL_LOG_CHANNEL_TYPE_DEFAULT)
    uEntryCount: int = 0   # buffer entry capacity (0 = 1024)

@dataclass(slots=True)
class plWindowIcon:
    # tightly packed, straight-alpha RGBA8 (len == uWidth * uHeight * 4); each dimension 1..256
    uWidth: int = 0
    uHeight: int = 0
    pucPixels: bytes | bytearray | memoryview = b""

@dataclass(slots=True)
class plWindowTitleBarDesc:
    uHeight: int = 0                  # logical pixels at 96 DPI; 0 = 32, otherwise 24..96
    uBackgroundColor: int = 0         # colors: 0 = default; otherwise 0xAABBGGRR (PL_COLOR_32_RGBA), alpha ignored
    uInactiveBackgroundColor: int = 0
    uTextColor: int = 0
    uAccentColor: int = 0             # thin window border and title-bar separator
    uBrandColor: int = 0              # optional short accent below the brand
    uStatusColor: int = 0             # status dot; 0 = muted teal
    acBrand: str = ""                 # optional brand (max 63 UTF-8 bytes); enables left-aligned breadcrumb layout
    acTitle: str = ""                 # optional title override (max 255 UTF-8 bytes); empty uses window title
    acStatus: str = ""                # optional status (max 63 UTF-8 bytes); hidden when space is limited

@dataclass(slots=True)
class plSphere:
    fRadius: float
    tCenter: plVec3

@dataclass(slots=True)
class plCapsule:
    tBasePos: plVec3
    tTipPos: plVec3
    fRadius: float

@dataclass(slots=True)
class plCylinder:
    tBasePos: plVec3
    tTipPos: plVec3
    fRadius: float

@dataclass(slots=True)
class plCone:
    tBasePos: plVec3
    tTipPos: plVec3
    fRadius: float

@dataclass(slots=True)
class plDrawFrustumDesc:
    fYFov: float
    fAspectRatio: float
    fNearZ: float
    fFarZ: float

# @dataclass(slots=True)
class plCameraPerspectiveDesc:
    eDepthMode: plCameraDepthMode
    fYFov: float
    fAspectRatio: float
    fNearZ: float
    fFarZ: float

@dataclass(slots=True)
class plCameraOrthographicDesc:
    eDepthMode: plCameraDepthMode
    fWidth: float
    fHeight: float
    fNearZ: float
    fFarZ: float