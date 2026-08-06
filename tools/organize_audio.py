import os
import shutil

ASSETS_DIR = r"H:\Project\LDMP_Project\LDoE_Engine\assets"
AUDIO_DIR = os.path.join(ASSETS_DIR, "audio")
MUSIC_DIR = os.path.join(ASSETS_DIR, "music")
ACTORS_DIR = os.path.join(ASSETS_DIR, "actors")
LEGACY_DIR = os.path.join(AUDIO_DIR, "legacy_archive")

def organize():
    if os.path.exists(LEGACY_DIR):
        if os.path.exists(ACTORS_DIR):
            for item in os.listdir(LEGACY_DIR):
                shutil.move(os.path.join(LEGACY_DIR, item), os.path.join(ACTORS_DIR, item))
            shutil.rmtree(LEGACY_DIR)
        else:
            os.rename(LEGACY_DIR, ACTORS_DIR)
        print("Папка legacy_archive успешно переименована/перенесена в 'actors'.")

    if os.path.exists(AUDIO_DIR):
        for file in os.listdir(AUDIO_DIR):
            file_path = os.path.join(AUDIO_DIR, file)
            if os.path.isfile(file_path) and file.lower().endswith(('.ogg', '.mp3', '.wav')):
                if file[0].isdigit() or "space" in file.lower() or "mech" in file.lower() or "docking" in file.lower():
                    dest_path = os.path.join(MUSIC_DIR, file)
                    if not os.path.exists(dest_path):
                        shutil.move(file_path, dest_path)
                    else:
                        os.remove(file_path)
        print("Музыкальные треки распределены в папки назначения.")

if __name__ == "__main__":
    organize()
    print("Организация структуры аудио завершена!")
