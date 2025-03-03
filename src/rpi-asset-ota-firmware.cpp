/*
 * Project myProject
 * Author: Your Name
 * Date:
 * For comprehensive documentation and examples, please visit:
 * https://docs.particle.io/firmware/best-practices/firmware-template/
 */

// Include Particle Device OS APIs
#include "Particle.h"

// Let Device OS manage the connection to the Particle Cloud
SYSTEM_MODE(AUTOMATIC);

// Run the application and system concurrently in separate threads
// PRODUCT_VERSION(1);

void handleAssets(spark::Vector<ApplicationAsset> assets);
STARTUP(System.onAssetOta(handleAssets));

// Show system, cloud connectivity, and application logs over USB
// View logs with CLI using 'particle serial monitor --follow'
SerialLogHandler logHandler(LOG_LEVEL_NONE, {{"app", LOG_LEVEL_ALL}});

void handleAssets(spark::Vector<ApplicationAsset> assets)
{
  Log.info("Processing assets...");
  for (ApplicationAsset &asset : assets)
  {
    Log.info("Asset name: %s", asset.name().c_str());
    if (asset.name() == "app.py")
    {
      Log.info("Python file found, sending to serial");

      // Reset the asset to ensure we're at the beginning
      asset.reset();

      // Create a buffer for reading chunks
      const size_t BUFFER_SIZE = 128;
      char buffer[BUFFER_SIZE];

      // Print a header to serial
      Serial.printlnf("--- Begin %s (size: %d bytes) ---", asset.name().c_str(), asset.size());

      // Read and send data in chunks
      while (asset.available() > 0)
      {
        int bytesRead = asset.read(buffer, BUFFER_SIZE - 1);
        if (bytesRead > 0)
        {
          // Write directly to serial
          Serial.write((const uint8_t *)buffer, bytesRead);
        }
      }

      // Print a footer to serial
      Serial.println("\n--- End of file ---");

      // Add a small delay to ensure the data has been sent
      delay(100);
    }
  }
  System.assetsHandled(true);
}

void setup()
{
  Serial.begin(115200);
  waitFor(Serial.isConnected, 10000);
  delay(2000);

  // redo assets handling on next boot for demo purposes
  // This is just here to make it easier to see the early log messages on
  // the USB serial debug. You probably don't want this in production code.
  handleAssets(System.assetsAvailable());
}

void loop()
{
}
