import os
import glob
import static_ffmpeg
from pydub import AudioSegment

def convert_mp3_to_wav():
    static_ffmpeg.add_paths()
    music_dir = os.path.expanduser('~/Downloads/music')
    mp3_files = glob.glob(os.path.join(music_dir, '*.mp3'))
    
    print(f"Found {len(mp3_files)} MP3 files in {music_dir} to convert.")
    
    for mp3_path in mp3_files:
        base_name = os.path.splitext(os.path.basename(mp3_path))[0]
        wav_path = os.path.join(music_dir, f"{base_name}.wav")
        print(f"Converting: {os.path.basename(mp3_path)} -> {os.path.basename(wav_path)} ...")
        
        try:
            sound = AudioSegment.from_file(mp3_path)
            # Set to 16kHz Mono 16-bit PCM for Edge Impulse / TinyML optimal compatibility
            sound = sound.set_frame_rate(16000).set_channels(1)
            sound.export(wav_path, format="wav")
            print(f"✓ Converted {os.path.basename(wav_path)} successfully!")
        except Exception as e:
            print(f"✗ Failed to convert {os.path.basename(mp3_path)}: {e}")

if __name__ == "__main__":
    convert_mp3_to_wav()
