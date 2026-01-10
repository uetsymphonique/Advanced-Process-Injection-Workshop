# Build Guide: CWLProcessGhosting

## Requirements

- Visual Studio 2019/2022 or Build Tools
- MSBuild (usually included with Visual Studio)
- Windows SDK 10.0

## Method 1: Build from Solution File (Recommended)

### Build Release x64

```bash
msbuild CWLProcessGhosting.sln /p:Configuration=Release /p:Platform=x64
```

### Build Debug x64

```bash
msbuild CWLProcessGhosting.sln /p:Configuration=Debug /p:Platform=x64
```

### Rebuild (Clean + Build)

```bash
msbuild CWLProcessGhosting.sln /p:Configuration=Release /p:Platform=x64 /t:Rebuild
```

## Method 2: Build from Project File

### Build Release x64

```bash
msbuild CWLProcessGhosting\CWLProcessGhosting.vcxproj /p:Configuration=Release /p:Platform=x64
```

### Rebuild

```bash
msbuild CWLProcessGhosting\CWLProcessGhosting.vcxproj /p:Configuration=Release /p:Platform=x64 /t:Rebuild
```

## Method 3: Using Developer Command Prompt

1. Open **Developer Command Prompt for VS** from Start Menu
2. Navigate to project directory:

```bash
cd D:\vcs\Advanced-Process-Injection-Workshop\CWLProcessGhosting
```

3. Build:

```bash
msbuild CWLProcessGhosting.sln /p:Configuration=Release /p:Platform=x64
```

## Method 4: Manual MSBuild Path

If MSBuild is not in PATH, use full path:

### Visual Studio 2022

```bash
"C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\MSBuild.exe" CWLProcessGhosting.sln /p:Configuration=Release /p:Platform=x64
```

### Visual Studio 2019

```bash
"C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\MSBuild\Current\Bin\MSBuild.exe" CWLProcessGhosting.sln /p:Configuration=Release /p:Platform=x64
```

## Output Location

After successful build, executable will be located at:

- **Release x64**: `x64\Release\CWLProcessGhosting.exe`
- **Debug x64**: `x64\Debug\CWLProcessGhosting.exe`

## Build Options

### Configuration

- `Release`: Optimized build, no debug symbols
- `Debug`: Build with debug symbols

### Platform

- `x64`: 64-bit (recommended)
- `Win32`: 32-bit

### Targets

- `Build`: Build only changed files
- `Rebuild`: Clean then build from scratch
- `Clean`: Remove all build files

### Custom Payload Path (Compile-Time)

By default, the payload path is `C:\temp\payload64.exe`. You can override this at compile time:

```bash
# Custom payload path - Use double backslashes!
msbuild CWLProcessGhosting.sln /p:Configuration=Release /p:Platform=x64 /p:CustomPayloadPath="D:\\malware\\payload.exe"

# Another example
msbuild CWLProcessGhosting.sln /p:Configuration=Release /p:Platform=x64 /p:CustomPayloadPath="C:\\Users\\YourName\\Desktop\\implant.exe"

# With Rebuild
msbuild CWLProcessGhosting.sln /p:Configuration=Release /p:Platform=x64 /p:CustomPayloadPath="C:\\temp\\implant.exe" /t:Rebuild
```

**Important Notes:**

- **Must use double backslashes `\\`** in the path (otherwise `\t` becomes tab, `\n` becomes newline, etc.)
- No need for `L\"` prefix
- The path is automatically converted to a wide string literal at compile time

## Example Build Commands

### Build Release with verbose output

```bash
msbuild CWLProcessGhosting.sln /p:Configuration=Release /p:Platform=x64 /v:detailed
```

### Build with multiple cores (parallel build)

```bash
msbuild CWLProcessGhosting.sln /p:Configuration=Release /p:Platform=x64 /m
```

### Clean before build

```bash
msbuild CWLProcessGhosting.sln /p:Configuration=Release /p:Platform=x64 /t:Clean
msbuild CWLProcessGhosting.sln /p:Configuration=Release /p:Platform=x64 /t:Build
```
