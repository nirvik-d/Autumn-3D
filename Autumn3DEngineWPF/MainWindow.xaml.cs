using System;
using System.Windows;
using Microsoft.Win32;
using Autumn3DWrapper;
using System.Threading.Tasks;

namespace Autumn3DEngineWPF
{
    public partial class MainWindow : Window
    {
        private string selectedModelPath;

        public MainWindow()
        {
            InitializeComponent();
        }

        private void OnLoadModelClick(object sender, RoutedEventArgs e)
        {
            OpenFileDialog openFileDialog = new OpenFileDialog
            {
                Title = "Select a 3D Model",
                Filter = "GLTF Files (*.gltf;*.glb)|*.gltf;*.glb",
                CheckFileExists = true,
                CheckPathExists = true
            };

            if (openFileDialog.ShowDialog() == true)
            {
                selectedModelPath = openFileDialog.FileName;
                MessageBox.Show($"Model loaded: {selectedModelPath}", "Model Loaded", MessageBoxButton.OK, MessageBoxImage.Information);
            }
        }

        private async void OnRenderClick(object sender, RoutedEventArgs e)
        {
            if (string.IsNullOrEmpty(selectedModelPath))
            {
                MessageBox.Show("Please load a model first.", "Error", MessageBoxButton.OK, MessageBoxImage.Warning);
                return;
            }

            await Task.Run(() =>
            {
                try
                {
                    RendererWrapper rendererWrapper = new RendererWrapper();
                    rendererWrapper.CreateGLFWWindow(800, 600); // Must stay on the rendering thread
                    rendererWrapper.InitializeOpenGL();        // OpenGL context initialization
                    rendererWrapper.Load3DModel(selectedModelPath);
                    rendererWrapper.Render();                  // Starts the rendering process
                }
                catch (Exception ex)
                {
                    // Capture any exceptions and report back to the UI thread
                    Application.Current.Dispatcher.Invoke(() =>
                    {
                        MessageBox.Show($"Rendering failed: {ex.Message}", "Error", MessageBoxButton.OK, MessageBoxImage.Error);
                    });
                }
            });
        }

    }
}
