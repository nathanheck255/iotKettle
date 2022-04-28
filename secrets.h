//#include <pgmspace.h>
 
#define SECRET
#define THINGNAME "NathansESP32"
 
const char WIFI_SSID[] = "NathansPhone";
const char WIFI_PASSWORD[] = "testtest";
const char AWS_IOT_ENDPOINT[] = "a3g4e66hhemyfb-ats.iot.us-east-1.amazonaws.com";
 
// Amazon Root CA 1
static const char AWS_CERT_CA[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIDQTCCAimgAwIBAgITBmyfz5m/jAo54vB4ikPmljZbyjANBgkqhkiG9w0BAQsF
ADA5MQswCQYDVQQGEwJVUzEPMA0GA1UEChMGQW1hem9uMRkwFwYDVQQDExBBbWF6
b24gUm9vdCBDQSAxMB4XDTE1MDUyNjAwMDAwMFoXDTM4MDExNzAwMDAwMFowOTEL
MAkGA1UEBhMCVVMxDzANBgNVBAoTBkFtYXpvbjEZMBcGA1UEAxMQQW1hem9uIFJv
b3QgQ0EgMTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBALJ4gHHKeNXj
ca9HgFB0fW7Y14h29Jlo91ghYPl0hAEvrAIthtOgQ3pOsqTQNroBvo3bSMgHFzZM
9O6II8c+6zf1tRn4SWiw3te5djgdYZ6k/oI2peVKVuRF4fn9tBb6dNqcmzU5L/qw
IFAGbHrQgLKm+a/sRxmPUDgH3KKHOVj4utWp+UhnMJbulHheb4mjUcAwhmahRWa6
VOujw5H5SNz/0egwLX0tdHA114gk957EWW67c4cX8jJGKLhD+rcdqsq08p8kDi1L
93FcXmn/6pUCyziKrlA4b9v7LWIbxcceVOF34GfID5yHI9Y/QCB/IIDEgEw+OyQm
jgSubJrIqg0CAwEAAaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNVHQ8BAf8EBAMC
AYYwHQYDVR0OBBYEFIQYzIU07LwMlJQuCFmcx7IQTgoIMA0GCSqGSIb3DQEBCwUA
A4IBAQCY8jdaQZChGsV2USggNiMOruYou6r4lK5IpDB/G/wkjUu0yKGX9rbxenDI
U5PMCCjjmCXPI6T53iHTfIUJrU6adTrCC2qJeHZERxhlbI1Bjjt/msv0tadQ1wUs
N+gDS63pYaACbvXy8MWy7Vu33PqUXHeeE6V/Uq2V8viTO96LXFvKWlJbYK8U90vv
o/ufQJVtMVT8QtPHRh8jrdkPSHCa2XV4cdFyQzR1bldZwgJcJmApzyMZFo6IQ6XU
5MsI+yMRQ+hDKXJioaldXgjUkK642M4UwtBV8ob2xJNDd2ZhwLnoQdeXeGADbkpy
rqXRfboQnoZsG4q5WTP468SQvvG5
-----END CERTIFICATE-----
)EOF";
 
// Device Certificate                                               //change this
static const char AWS_CERT_CRT[] PROGMEM = R"KEY(
-----BEGIN CERTIFICATE-----
MIIDWjCCAkKgAwIBAgIVALj6ykCZOqXbOBPoQz3CKSx3oqQiMA0GCSqGSIb3DQEB
CwUAME0xSzBJBgNVBAsMQkFtYXpvbiBXZWIgU2VydmljZXMgTz1BbWF6b24uY29t
IEluYy4gTD1TZWF0dGxlIFNUPVdhc2hpbmd0b24gQz1VUzAeFw0yMjA0MDYyMTAy
NDBaFw00OTEyMzEyMzU5NTlaMB4xHDAaBgNVBAMME0FXUyBJb1QgQ2VydGlmaWNh
dGUwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQDNoYXFUEVZ829so/Mv
/AvzRGcog1bszXahWYXDEeYW4mRVIUBmEn4NTFizKHYabBI7KOMTolo4kxf/LSjr
nN332I5V0FXPdgIARc3HdWKhFQ7NCNErKY/GPG0RUdFJ8X3lSEOeLxTBgJS9MoNF
vDKJzt48UqgSJ902AqkaJsl2Wmq2mADOAFoHyzG9TVQf51JfOA/NthzbyNh2PPdg
+2jNzz4iXAXfQEYHOeznEiNNMGSXylOPeE+Hk6FGoXObWjkW4yFD+FLLW0GmlcDo
yoRkX1GynnhetgEOaOp05md35OKhNxf/1PBsJ5cwVF3CToime8UJsRx4bd5ZNzeQ
SSbFAgMBAAGjYDBeMB8GA1UdIwQYMBaAFDV0IQzv0NRrjUsVfTb1TsI1emGQMB0G
A1UdDgQWBBR+qFTCRU7hEdeG5/vw5JmPj+c+JzAMBgNVHRMBAf8EAjAAMA4GA1Ud
DwEB/wQEAwIHgDANBgkqhkiG9w0BAQsFAAOCAQEAomEc168W/xwCDD4jH5Ymki8T
YgxPuGE/BbX465gOmad7a/7rmssEJyshQnt+hgrlOkj55azkNP/zEVyantksWCB7
af8alK6aZ9yCNLhhn0fWxk7d6VTVC3B1fvdRhrQtsYEY7oGv8ws0IBxgKd5PJ0/8
1tjgwhd3QGjBUI6LKTc3QFO+7hQPqm9D+ONC2b+5H3QY3tXfYcuI0w7uGZ5cHAlj
KWU7LXPbiXzVsEnTOzsAEuDpSXqjSH0y8/dLAfxx7Um7zTZSMoIhDk61oogTxWvl
428hoKxzTdUON9ATikdHfkdG1UEhkqGv+loG8O+m6LMothNtgwXOi8exxmPeLA==
-----END CERTIFICATE----- 
)KEY";
 
// Device Private Key                                               //change this
static const char AWS_CERT_PRIVATE[] PROGMEM = R"KEY(
-----BEGIN RSA PRIVATE KEY-----
MIIEpAIBAAKCAQEAzaGFxVBFWfNvbKPzL/wL80RnKINW7M12oVmFwxHmFuJkVSFA
ZhJ+DUxYsyh2GmwSOyjjE6JaOJMX/y0o65zd99iOVdBVz3YCAEXNx3VioRUOzQjR
KymPxjxtEVHRSfF95UhDni8UwYCUvTKDRbwyic7ePFKoEifdNgKpGibJdlpqtpgA
zgBaB8sxvU1UH+dSXzgPzbYc28jYdjz3YPtozc8+IlwF30BGBzns5xIjTTBkl8pT
j3hPh5OhRqFzm1o5FuMhQ/hSy1tBppXA6MqEZF9Rsp54XrYBDmjqdOZnd+TioTcX
/9TwbCeXMFRdwk6IpnvFCbEceG3eWTc3kEkmxQIDAQABAoIBAFzRQd6Uoxk9Jth1
ELpiHMrL33yL/FqbS6b9D8HDfIZr3joG7liklD1QbuE/rD+09G27wPQF3o+W5cgL
DGN9xTuwhl750SH59GFHIOGdfM2VAb9O8Yt4ESu77sdYlQ25cchi3BK00bueXIUi
HdsGXhGSTQIRlUfMsq9EeCwTwfAnhAKRyZ39nAmhF8cw0a4oe6l8ZHD5ebOEpYVo
VTiI/ejze8wMxdUCXN1PM/xXVNF6ZQzHaarGVhuz8mKeRudyf6bEdsMI/JdUkFPC
k4UewPrUXkQR3/X8CjJDdE1zP9d29sH/C/FT1hcIcNEF1ek4qehTxsFfNBePv42/
qTMJR6kCgYEA+ncQGSdlv+5DVSCloCpHaf8O/ZmXWSf5vWas3OZv/JDBmP3lrALi
SrOxfs5SZTq6T51mJ0VXFVNzkjVWldNjyoFvEgL5dGYzWawChXMGGFqYbUkZRE1D
W3J5GJUSk1qxUwBWjHpXUdt6NaDysgR/vqReCKq9IClvvNqWW8Y+1TsCgYEA0izS
p9+WhF4hDvU7OjDDmRa5DjYB6A65Vz1X5kVSO+GEeV//DJm/ZCmhTri2u3x6fam3
LW4luEGSq6WCysMfIWWzOEuJyNgavOWXGR+6S8wdYyLiyVluWl6Q99KS05ayApQj
6FcORBKEFf+vtClEXmEw1ZQWo/Z7XE+cG6YoM/8CgYEA7kzt4iDb7FqzcYEYVGUl
gTINfzftCgTA1nWkLqCJGBJaQEgBh0FPMKf7TpzbRDk2Hp2xHdgZXM8DfK6G7RIt
NfUkSkzOLvM2FDFWTMX9nJpFwGT2R0gDuDQYB01+WPjHEiOWNR4JinZ5tiCmCL3D
FQ2p8jEQtFL4XwTSXgsmqRUCgYAQAloJb36HlqPt/8ADLrO3dYOjEXoTswtrGcEY
SS96YDM3C5jh3NQ3TPR8ipVgspqh98t8Kme1IUmKyZCs4S/920FLiHV2Vv+yY8qe
n0wRefrDEmUz8Rv2xj1Eo0WL2G62w5H4oKj1hnOJOXCoyHvstHqDykPvOVcsMOQz
NvTZAQKBgQDhyybNZR8OSskLrBrVtCc0q861SLVlqbvXXU95L5ZMdBX/3c9cI0mJ
DfH5lAeGb16Yx+3+yXetxJkA/CKjhHLrMWQP8IfnLkGmihVtmK2TcSH0nFz4n6Vc
tOpbHmTVbQrGS8hy85QyrZ0dz2uUSILBlnEOxaHcjV114piA4QVYsw==
-----END RSA PRIVATE KEY----- 
)KEY";