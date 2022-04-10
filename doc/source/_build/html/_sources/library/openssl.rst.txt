#########
OpenSSL
#########

.. contents::
    :local:

X.509 Certificate
====================

.. code-block::

   openssl req -x509 -newkey rsa:4096 -keyout key.pem -out cert.pem -days 365
