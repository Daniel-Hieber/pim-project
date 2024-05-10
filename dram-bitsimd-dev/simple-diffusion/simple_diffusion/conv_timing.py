import torch
import torch.nn as nn
import torchvision.transforms as transforms
import torchvision.datasets as datasets
from torch.utils.data import DataLoader, Dataset
import time
import matplotlib.pyplot as plt

class TransformedDataset(Dataset):
    def __init__(self, original_dataset, transform=None):
        self.original_dataset = original_dataset
        self.transform = transform

    def __len__(self):
        return len(self.original_dataset)

    def __getitem__(self, idx):
        image, label = self.original_dataset[idx]
        if self.transform:
            image = self.transform(image)
        return image, label

transform = transforms.Compose([
    transforms.Resize((128, 128)),  # Resize to 128x128
    transforms.ToTensor(),  
    transforms.Normalize((0.5, 0.5, 0.5), (0.5, 0.5, 0.5))
])

cifar100 = datasets.CIFAR100(root='./data', train=True, download=True)
transformed_cifar100 = TransformedDataset(cifar100, transform=transform)

conv_layer = nn.Conv2d(in_channels=3, out_channels=3, kernel_size=3, stride=1, padding=1)
device = torch.device("cuda" if torch.cuda.is_available() else "cpu")
conv_layer = conv_layer.to(device)

def time_processing(num_images):
    loader = DataLoader(transformed_cifar100, batch_size=1, shuffle=False, num_workers=0)
    cpu_times = []
    gpu_times = []

    for i, (image, _) in enumerate(loader):
        if i >= num_images:
            break
        image = image.to(device)

        if torch.cuda.is_available():
            torch.cuda.synchronize()
            start_time_gpu = time.time()
            output_gpu = conv_layer(image)
            torch.cuda.synchronize()
            gpu_times.append(time.time() - start_time_gpu)

        conv_layer.to('cpu')
        image_cpu = image.to('cpu')
        start_time_cpu = time.time()
        output_cpu = conv_layer(image_cpu)
        cpu_times.append(time.time() - start_time_cpu)

        conv_layer.to(device)

    avg_cpu_time = sum(cpu_times) / len(cpu_times) if cpu_times else 0
    avg_gpu_time = sum(gpu_times) / len(gpu_times) if gpu_times else 0
    return avg_cpu_time, avg_gpu_time

image_counts = [1, 10, 100, 1000]
cpu_times = []
gpu_times = []

for count in image_counts:
    avg_cpu_time, avg_gpu_time = time_processing(count)
    cpu_times.append(avg_cpu_time)
    gpu_times.append(avg_gpu_time)

# Plotting
plt.figure(figsize=(10, 5))
plt.plot(image_counts, cpu_times, label='CPU Times', marker='o')
plt.plot(image_counts, gpu_times, label='GPU Times', marker='o')
plt.xlabel('Number of Images')
plt.ylabel('Average Time (s)')
plt.title('Average Processing Time on CPU vs GPU')
plt.xscale('log')
plt.yscale('log')
plt.legend()
plt.grid(True)
plt.show()
