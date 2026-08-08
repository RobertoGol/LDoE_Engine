$sourceDir = ".\" 
$targetDir = ".\assets"

Write-Host "Начинаем умную сортировку ассетов LDoE..." -ForegroundColor Cyan

# Создаем базовую директорию assets, если ее нет
if (-not (Test-Path $targetDir)) { 
    New-Item -ItemType Directory -Path $targetDir -Force | Out-Null 
}

# Получаем все файлы, исключая те, что уже отсортированы в assets
$allFiles = Get-ChildItem -Path $sourceDir -File -Recurse | Where-Object { $_.FullName -notmatch "\\assets\\" }

foreach ($file in $allFiles) {
    $ext = $file.Extension.ToLower()
    $parent = $file.Directory.Name
    $destFolder = ""

    # 1. Архивы в корень assets
    if ($ext -match '\.(rar|zip|7z)$') {
        $destFolder = $targetDir
    }
    # 2. JSON файлы (Unity Dumps разделяют их по папкам, сохраняем эту логику)
    elseif ($ext -eq ".json") {
        if ($parent -match "Animator") { $destFolder = Join-Path $targetDir "Animator" }
        elseif ($parent -match "MonoBehaviour") { $destFolder = Join-Path $targetDir "MonoBehaviour" }
        else { $destFolder = Join-Path $targetDir "Materials" }
    }
    # 3. 3D Модели
    elseif ($ext -match '\.(obj|mesh|fbx)$') {
        $destFolder = Join-Path $targetDir "Mesh"
    }
    # 4. Аудио
    elseif ($ext -match '\.(m4a|mp3|wav|ogg)$') {
        $destFolder = Join-Path $targetDir "Audio"
    }
    # 5. Текстуры
    elseif ($ext -match '\.(png|jpg|tga|jpeg)$') {
        $destFolder = Join-Path $targetDir "Textures"
    }
    # 6. Шейдеры (оригинальные из игры)
    elseif ($ext -eq ".shader") {
        $destFolder = Join-Path $targetDir "Shaders"
    }
    # 7. Шрифты
    elseif ($ext -match '\.(ttf|otf)$') {
        $destFolder = Join-Path $targetDir "Fonts"
    }
    # 8. Библиотеки и код
    elseif ($ext -match '\.(dll|so|cs)$') {
        $destFolder = Join-Path $targetDir "Libraries"
    }
    # 9. Метаданные и прочее
    elseif ($ext -match '\.(xml|csv|txt|dat)$') {
        $destFolder = Join-Path $targetDir "Meta"
    }
    else {
        # Игнорируем скрипты сборки и неизвестный мусор
        continue
    }

    # Создаем папку назначения, если её еще нет
    if (-not (Test-Path $destFolder)) {
        New-Item -ItemType Directory -Path $destFolder -Force | Out-Null
    }

    # Перемещаем файл
    Move-Item -Path $file.FullName -Destination $destFolder -Force
}

# Аккуратно переносим наши самописные шейдеры в корень assets, чтобы C++ код движка сразу их нашел
Move-Item -Path ".\basic.vert" -Destination ".\assets\basic.vert" -Force -ErrorAction SilentlyContinue
Move-Item -Path ".\basic.frag" -Destination ".\assets\basic.frag" -Force -ErrorAction SilentlyContinue

Write-Host "Сортировка успешно завершена! Вся матрешка разобрана по категориям." -ForegroundColor Green