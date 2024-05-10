import torch
import torch.nn as nn
import torch.optim as optim
from torch.utils.data import DataLoader, TensorDataset, Subset
import torchvision
import torchvision.transforms as transforms
from modeling_uvit import Mish, Upsample2D
import numpy as np
import time


# Define a simple model using the provided components
class SimpleModel(nn.Module):
    def __init__(self):
        super(SimpleModel, self).__init__()
        self.up = Upsample2D(channels=3, use_conv=True)  # Assuming this upsamples to the same number of channels
        self.activation = Mish()
        self.classifier = nn.Linear(in_features=12288, out_features=10)  # Adjust in_features to match output of last layer before this


    def forward(self, x):
        start_time = time.time()  
        x = self.up(x)
        elapsed_time = time.time() - start_time 

        x = self.activation(x)
        x = x.view(x.size(0), -1)  # Flatten the output to match [batch_size, num_features]
        x = self.classifier(x)

        return x, elapsed_time


# Data transforms
transform = transforms.Compose(
    [transforms.ToTensor(),
     transforms.Normalize((0.5, 0.5, 0.5), (0.5, 0.5, 0.5))])


if __name__ == '__main__':
    # Hyperparameters
    batch_size = 128
    num_dev_samples = 50000  # Total is 50,000 for CIFAR-10
    lr = 0.001
    step_size = 5
    times = []


    trainset = torchvision.datasets.CIFAR10(root='./data', train=True, download=True, transform=transform)
    trainset = Subset(trainset, range(num_dev_samples))
    # trainloader = DataLoader(trainset, batch_size=batch_size, shuffle=True, num_workers=2)
    # Splitting trainset for validation
    num_train = len(trainset)
    indices = list(range(num_train))
    split = int(np.floor(0.1 * num_train))


    np.random.shuffle(indices)


    train_idx, val_idx = indices[split:], indices[:split]
    train_subset = Subset(trainset, train_idx)
    val_subset = Subset(trainset, val_idx)


    trainloader = DataLoader(train_subset, batch_size=batch_size, shuffle=True, num_workers=2)
    valloader = DataLoader(val_subset, batch_size=batch_size, shuffle=False, num_workers=2)


    testset = torchvision.datasets.CIFAR10(root='./data', train=False, download=True, transform=transform)
    testloader = DataLoader(testset, batch_size=batch_size, shuffle=False, num_workers=2)


    # Model, Loss and Optimizer
    model = SimpleModel()
    criterion = nn.CrossEntropyLoss()
    optimizer = optim.Adam(model.parameters(), lr=lr)
    scheduler = torch.optim.lr_scheduler.StepLR(optimizer, step_size=step_size, gamma=0.1)  # Adjusts the lr every step_size epochs


    # Training loop
    # def train_model():
    #     model.train()
    #     for epoch in range(5):
    #         for data, target in trainloader:
    #             optimizer.zero_grad()
    #             output = model(data)
    #             loss = criterion(output, target)
    #             loss.backward()
    #             optimizer.step()
    #         print(f'Epoch {epoch+1}, Loss: {loss.item()}')
    #         scheduler.step()  # Adjust the learning rate
   
    # train_model()


    # Training and validation loop
    def train_and_validate_model():
        model.train()
        for epoch in range(3):
            for data, target in trainloader:
                optimizer.zero_grad()
                output, time_elapsed = model(data)
                times.append(time_elapsed)
                loss = criterion(output, target)
                loss.backward()
                optimizer.step()


            # Validation loss
            model.eval()
            val_loss = 0
            with torch.no_grad():
                for data, target in valloader:
                    output, _ = model(data)
                    val_loss += criterion(output, target).item()


            val_loss /= len(valloader)
            print(f'Epoch {epoch+1}, Training Loss: {loss.item()}, Validation Loss: {val_loss}')
            scheduler.step()


    train_and_validate_model()


    def test_model():
        model.eval()  # Set the model to evaluation mode
        correct = 0
        total = 0


        with torch.no_grad():  # No need to track gradients for testing
            for data, target in testloader:
                output, _ = model(data)
                _, predicted = torch.max(output.data, 1)  # Get the index of the max log-probability
                total += target.size(0)
                correct += (predicted == target).sum().item()


        accuracy = 100 * correct / total
        print(f'Accuracy of the model on the test images: {accuracy:.2f}%')
        print(f'Average time per forward pass: {np.mean(times)}')


    test_model()