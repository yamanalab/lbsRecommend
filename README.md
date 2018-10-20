# Privacy-Preserving Recommendation for Location-Based Services with Fully Homomorphic Encryption
Implementation of the protocol with the functions:
1. Service providers can compute aggregate information concerning user behavior patterns homomorphically, and send the encrypted results to PSP to ensure decryption, while maintaining the privacy of individual users. 
2. Encrypted database update.

## Deployment

Terminal1 - Initialization
```
$cd src/Initialization
$./Initialization
```
Terminal2:
Client
```
$cd src/client/
$./client
```
Terminal3:
Privacy-preserving Recommendation on LBS Server(PPRS)
```
$cd src/Server(PPRS)/
$./Server(PPRS)
```
Terminal4:
Privacy Service Provider(PSP):
```
$cd src/Server(PSP)/
$./Server(PSP)
```

## References
- HELib: https://github.com/shaih/HElib
