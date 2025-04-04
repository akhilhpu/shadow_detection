import os

image_files = []
os.chdir(os.path.join("data", "images", "train"))
for filename in os.listdir(os.getcwd()):
    if not filename.endswith(".txt") and not filename.endswith('.py'):
        image_files.append("data/images/train/" + filename)
os.chdir("..")
os.chdir("..")
with open("train.txt", "w") as outfile:
    for image in image_files:
        outfile.write(image)
        outfile.write("\n")
    outfile.close()
os.chdir("..")