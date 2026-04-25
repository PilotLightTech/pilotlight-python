from dataclasses import dataclass

class plShaderMacroDefinition:
    def __init__(self):
        self.pcName = None
        self.pcValue = None

class plShaderOptions:
    def __init__(self):
        self.tFlags = 0
        self.tOptimizationLevel = 0
        self.ptMacroDefinitions = []
        self.apcIncludeDirectories = []
        self.apcDirectories = []
        self.pcCacheOutputDirectory = None

@dataclass(slots=True)
class plDrawSolidOptions:
    uColor: int = 0xFFFFFFFF  # default white

@dataclass(slots=True)
class plDrawLineOptions:
    uColor: int = 0xFFFFFFFF  # default white
    fThickness: float = 1.0