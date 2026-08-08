$assetsPath = ".\assets"
# Строгий список папок, которые мы потрошим
$targetFolders = @("Libraries", "Materials", "split_data", "Textures", "DummyDll", "Data_for_sort")

# Базовые игровые категории
$categories = @("Characters", "Weapons", "Transport", "Props", "Locations", "Audio", "UI", "Libraries", "Other")
foreach ($c in $categories) {
    $path = Join-Path $assetsPath $c
    if (-not (Test-Path $path)) { New-Item -ItemType Directory -Path $path -Force | Out-Null }
}

Write-Host "Начинаем сборку бандлов .bda из указанных папок..." -ForegroundColor Yellow

foreach ($tf in $targetFolders) {
    $folderPath = Join-Path $assetsPath $tf
    if (-not (Test-Path $folderPath)) { continue }

    # Берем все файлы внутри целевой папки
    $files = Get-ChildItem -Path $folderPath -File -Recurse

    foreach ($file in $files) {
        $name = $file.Name.ToLower()
        $baseName = $file.BaseName # Имя файла без расширения

        # 1. Определяем логическую категорию
        $destCat = "Other"

        # Библиотеки и DLL не пакуем в бандлы, а просто складываем в Libraries
        if ($file.Extension -match "\.(dll|so|cs|pdb)$" -or $tf -match "Libraries|DummyDll") { $destCat = "Libraries" }
        elseif ($name -match "weapon|gun|rifle|bow|sword|axe|bat_|machete|cleaver|shotgun|glock|ak47|m16|grenade|c4|pistol|uzi") { $destCat = "Weapons" }
        elseif ($name -match "backpack|headwear|torso|legs|feet|panties|helmet|armor|suit|cloth|_man_|_woman_|unisex|hair|beard|head|axel|chare|testbody|cucker|character|zombie|bloater|spit|boss|infected|parasite|deer|bear|wolf|fox|corgi|husky|dog_|turkey|fish_|cat_|puppy|npc|trader|raider|mercenary") { $destCat = "Characters" }
        elseif ($name -match "car|pickup|tank|engine|bike|chopper|atv|boat|truck|scooter|helicopter|hovercraft") { $destCat = "Transport" }
        elseif ($name -match "bunker|farm|commune|cellar|factory|motel|police|port|swamp|crater|settlement|laboratory|arena|location") { $destCat = "Locations" }
        elseif ($name -match "chest|box|storage|safe|locker|workbench|table|furnace|campfire|smelter|generator|machine|furniture|decor|chair|bed|lamp|door|wall|floor|fence|window|roof|stairs|light|photo|radio|electro|pack") { $destCat = "Props" }
        elseif ($name -match "ui_|icon|button|menu|window|dialog") { $destCat = "UI" }
        elseif ($name -match "\.(m4a|wav|ogg)$") { $destCat = "Audio" }

        # 2. Формируем путь для перемещения
        if ($destCat -eq "Libraries") {
            $targetDir = Join-Path $assetsPath $destCat
        } else {
            # МАГИЯ: Все файлы с одинаковым именем сливаются в один .bda бандл!
            $bundleName = "$baseName.bda"
            $targetDir = Join-Path ($assetsPath) "$destCat\$bundleName"
        }

        if (-not (Test-Path $targetDir)) { New-Item -ItemType Directory -Path $targetDir -Force | Out-Null }

        $targetPath = Join-Path $targetDir $file.Name

        # 3. Перемещаем файл
        if ($file.FullName -ne $targetPath) {
            Move-Item -Path $file.FullName -Destination $targetPath -Force -ErrorAction SilentlyContinue
        }
    }
}

Write-Host "Удаляем пустые отработанные папки..." -ForegroundColor Cyan
foreach ($tf in $targetFolders) {
    $folderPath = Join-Path $assetsPath $tf
    if (Test-Path $folderPath) {
        # Удаляем вложенные пустые папки
        Get-ChildItem -Path $folderPath -Directory -Recurse | Where-Object { (Get-ChildItem -Path $_.FullName -File -Recurse | Measure-Object).Count -eq 0 } | Remove-Item -Recurse -Force -ErrorAction SilentlyContinue
        # Удаляем корневую папку, если она пуста
        if ((Get-ChildItem -Path $folderPath -File -Recurse | Measure-Object).Count -eq 0) {
            Remove-Item -Path $folderPath -Recurse -Force -ErrorAction SilentlyContinue
        }
    }
}

Write-Host "Сортировка завершена! Ассеты собраны в бандлы .bda" -ForegroundColor Green