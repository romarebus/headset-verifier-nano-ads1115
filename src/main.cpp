#include <Arduino.h>
#include <Shell.h>
#include <SPI.h>
#include <Adafruit_ADS1X15.h>


// Ideal values for R2: (Play = 0), (Vol+ = 240), (Vol- = 470), (Reserved = 135)

#define R_BIAS 2200                                   // ohms
#define V_BIAS 2200                                  // millivolts
#define ADC_RANGE 2048
#define ADC_MAX 32768  
//#define ADC_VREF 3000                               // milivolts
//#define ADC_RESOLUTION 10                          // bits
//#define ADC_MAX ((1<<ADC_RESOLUTION) - 1)         // bit shift @ 10 bits = 1023


int command_help(int argc, char** argv);
int command_test(int argc, char** argv);
int command_extra(int argc, char** argv);
int shell_reader(char * data);
void shell_writer(char data);


// Custom start message 
char* msg = (char*)" Type help to list commands.";


Adafruit_ADS1115 ads;  /* Use this for the 16-bit version */


void setup() {
    // Prepare serial communication
    Serial.begin(9600);
    // Wait after reset or power on...
    delay(1000);
    
    //Use External AREF
    // analogReference(EXTERNAL);
    // analogReference(0);

    // Initialize command line interface (CLI)
    // Here we pass the function pointers to the read and write functions.
    // We can also pass a char pointer to display a custom start message.
    //shell_init(shell_reader, shell_writer, 0);
    shell_init(shell_reader, shell_writer, msg);

    // Add commands to the shell
    shell_register(command_help, PSTR("help"));
    shell_register(command_test, PSTR("test"));
    shell_register(command_extra, PSTR("extra"));
    
    // The ADC input range (or gain) can be changed via the following
    // functions, but be careful never to exceed VDD +0.3V max, or to
    // exceed the upper and lower limits if you adjust the input range!
    // Setting these values incorrectly may destroy your ADC!
    //                                                                ADS1015  ADS1115
    //                                                                -------  -------
    // ads.setGain(GAIN_TWOTHIRDS);  // 2/3x gain +/- 6.144V  1 bit = 3mV      0.1875mV (default)
    // ads.setGain(GAIN_ONE);        // 1x gain   +/- 4.096V  1 bit = 2mV      0.125mV
    ads.setGain(GAIN_TWO);        // 2x gain   +/- 2.048V  1 bit = 1mV      0.0625mV
    // ads.setGain(GAIN_FOUR);       // 4x gain   +/- 1.024V  1 bit = 0.5mV    0.03125mV
    // ads.setGain(GAIN_EIGHT);      // 8x gain   +/- 0.512V  1 bit = 0.25mV   0.015625mV
    // ads.setGain(GAIN_SIXTEEN);    // 16x gain  +/- 0.256V  1 bit = 0.125mV  0.0078125mV

    if (!ads.begin()) {
    Serial.println("Failed to initialize ADS.");
    while (1);
  }
}


void loop() {
    // This should always be called to process user input.
    shell_task();
}


// Test commands: The commands should always use this function prototype.
// They receive 2 parameters: The total count of arguments (argc) and a pointer
// to the begining of each one of the null-terminated argument strings.
int command_help(int argc, char** argv) {
    // Print formatted text to terminal
    shell_println("-----------------------------------------------");
    shell_println("List of Commands:");
    shell_println("-----------------------------------------------");
    shell_println("");
    shell_println(" > help ");
    shell_println("");
    shell_println(" > test play ");
    shell_println(" > test vol+ ");
    shell_println(" > test vol- ");
    shell_println(" > test reserved ");
    shell_println("");
    shell_println(" > test 1 ");
    shell_println(" > test 2 ");
    shell_println(" > extra ");
    shell_println("");
    shell_println("-----------------------------------------------");
    shell_println("Exit...");
    return SHELL_RET_SUCCESS;
}


int command_test(int argc, char** argv) {
    //shell_printf("%d - \"%s\" [len:%d]\r\n", 1, argv[1], strlen(argv[1]) );
    // Print formatted text to terminal  
    shell_println("");
    shell_println("");
    shell_printf("Running Test %s...\r\n", argv[1] );
    shell_println("");

    if (strcmp(argv[1], "play") == 0) {
        shell_println("Please press and hold the play button for 5 seconds.");
        shell_println("Waiting for 5 seconds.");
        shell_println("");
        delay (5000);

        int32_t raw_v = ads.readADC_SingleEnded(0);
        int32_t vout = (raw_v * ADC_RANGE) / ADC_MAX;
        int32_t denom = V_BIAS - vout;
        int32_t r2 = (vout*R_BIAS)/denom;          // R2 = (Vout x R1) / (Vin - Vout)

        switch (r2) {
            case 0: {
                shell_println(" Play/Pause is in Spec");
                break;
            }
            case 1 ... 1000: {
                shell_println(" Play/Pause is not in Spec");
                break;
            }
            default: {
                shell_println(" Press the button you would like to test.");
                //shell_println(" r2:");
            }
        }
        shell_println("");
        shell_println("Exit...");
    }

    if (strcmp(argv[1], "vol+") == 0) {
        shell_println("Please press and hold the vol+ button for 5 seconds.");
        shell_println("Waiting for 5 seconds.");
        shell_println("");
        delay (5000);

        int32_t raw_v = ads.readADC_SingleEnded(0);
        int32_t vout = (raw_v * ADC_RANGE) / ADC_MAX;
        int32_t denom = V_BIAS - vout;
        int32_t r2 = (vout*R_BIAS)/denom;          // R2 = (Vout x R1) / (Vin - Vout)

        switch (r2) {
            case 0 ... 236: {
                shell_println(" Vol+ is not in Spec");
                break;
            }
            case 237 ... 243: {
                shell_println(" Vol+ is in Spec");
                break;
            }
            case 244 ... 1000: {
                shell_println(" Vol+ is not in Spec");
                break;
            }
            default: {
                shell_println(" Press the button you would like to test.");
                //shell_println(" r2:");
            }
        }
        shell_println("");
        shell_println("Exit...");
    }

    if (strcmp(argv[1], "vol-") == 0) {
        shell_println("Please press and hold vol- button for 5 seconds.");
        shell_println("Waiting for 5 seconds.");
        shell_println("");
        delay (5000);

        int32_t raw_v = ads.readADC_SingleEnded(0);
        int32_t vout = (raw_v * ADC_RANGE) / ADC_MAX;
        int32_t denom = V_BIAS - vout;
        int32_t r2 = (vout*R_BIAS)/denom;          // R2 = (Vout x R1) / (Vin - Vout)

        switch (r2) {
            case 0 ... 464: {
                shell_println(" Vol- is not in Spec");
                break;
            }
            case 465 ... 475: {
                shell_println(" Vol- is in Spec");
                break;
            }
            case 476 ... 1000: {
                shell_println(" Vol- is not in Spec");
                break;
            }
            default: {
                shell_println(" Press the button you would like to test.");
                //shell_println(" r2:");
            }
        }
        shell_println("");
        shell_println("Exit...");
    }

    if (strcmp(argv[1], "reserved") == 0) {
        shell_println("Please press and hold reserved button for 5 seconds.");
        shell_println("Waiting for 5 seconds.");
        shell_println("");
        delay (5000);

        int32_t raw_v = ads.readADC_SingleEnded(0);
        int32_t vout = (raw_v * ADC_RANGE) / ADC_MAX;
        int32_t denom = V_BIAS - vout;
        int32_t r2 = (vout*R_BIAS)/denom;          // R2 = (Vout x R1) / (Vin - Vout)

        switch (r2) {
            case 0 ... 132: {
                shell_println(" Reserved is not in Spec");
                break;
            }
            case 133 ... 137: {
                shell_println(" Reserved is in Spec");
                break;
            }
            case 138 ... 1000: {
                shell_println(" Reserved is not in Spec");
                break;
            }
            default: {
                shell_println(" Press the button you would like to test.");
                //shell_println(" r2:");
            }
        }
        shell_println("");
        shell_println("Exit...");
    }

    if (strcmp(argv[1], "1") == 0) {
        while (1) {  
            //shell_printf("Running Test %s...\r\n", argv[1] );

            int32_t raw_v = ads.readADC_SingleEnded(0);
            int32_t vout = (raw_v * ADC_RANGE) / ADC_MAX;
            int32_t denom = V_BIAS - vout;
            int32_t r2 = (vout*R_BIAS)/denom;      // R2 = (Vout x R1) / (Vin - Vout)

            shell_printf("r_v: %d", raw_v );
            shell_printf(" vout: %d", vout );
            shell_printf(" denom: %d", denom );
            shell_printf(" r2: %d\r\n", r2 );
        }
    }

    if (strcmp(argv[1], "2") == 0) {
        while (1) {

            int32_t raw_v = ads.readADC_SingleEnded(0);
            int32_t vout = (raw_v * ADC_RANGE) / ADC_MAX;
            int32_t denom = V_BIAS - vout;
            int32_t r2 = (vout*R_BIAS)/denom;      // R2 = (Vout x R1) / (Vin - Vout)

            switch (r2) {
                case 0: {
                    shell_println(" Play/Pause is in Spec");
                    break;
                }
                case 237 ... 243: {
                    shell_println(" Vol+ is in Spec");
                    break;
                }
                case 465 ... 475: {
                    shell_println(" Vol- is in Spec");
                    break;
                }
                case 133 ... 137: {
                    shell_println(" Reserved is in Spec");
                    break;
                }
                default: {
                    shell_println(" Press the button you would like to test.");
                    //shell_println(" r2:");
                }
            }
        }
    }

    return SHELL_RET_SUCCESS;
}


int command_extra(int argc, char** argv) {
    shell_println("Running \"extra\" now");
    shell_println("Exit...");
    return SHELL_RET_SUCCESS;
}


// Function to read data from serial port
// Functions to read from physical media should use this prototype:
// int my_reader_function(char * data)
int shell_reader(char * data) {
    // Wrapper for Serial.read() method
    if (Serial.available()) {
        *data = Serial.read();
        return 1;
    }
    return 0;
}


// Function to write data to serial port
// Functions to write to physical media should use this prototype:
// void my_writer_function(char data)
void shell_writer(char data) {
    // Wrapper for Serial.write() method
    Serial.write(data);
}
