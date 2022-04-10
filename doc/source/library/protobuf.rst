Protobuf
===================

.. contents::
    :local:

数据的序列化可以是文本，也可以是二进制，主要考虑是体积小，解析效率高。

Example:

.. code-block::

    message SearchRequest {
        required string query = 1;
        optional int32 page_number = 2;
        optional int32 result_per_page = 3 [default = 10];
        enum Corpus {
            UNIVERSAL = 0;
            WEB = 1;
            IMAGES = 2;
            LOCAL = 3;
            NEWS = 4;
            PRODUCTS = 5;
            VIDEO = 6;
        }
        optional Corpus corpus = 4 [default = UNIVERSAL];
    }


Defining A Message Type
---------------------------

Specifying Field Types
~~~~~~~~~~~~~~~~~~~~~~~~~~~

* string
* int32

Assigning Field Numbers
~~~~~~~~~~~~~~~~~~~~~~~~~~~
As you can see, each field in the message definition has a unique number. 
These numbers are used to identify your fields in the message binary format, and should not be changed once your message type is in use


Specifying Field Rules
~~~~~~~~~~~~~~~~~~~~~~~~~~~
You specify that message fields are one of the following:

* required: a well-formed message must have exactly one of this field.
* optional: a well-formed message can have zero or one of this field (but not more than one).
* repeated: this field can be repeated any number of times (including zero) in a well-formed message. The order of the repeated values will be preserved.

Compile proto file 
---------------------------

https://github.com/protocolbuffers/protobuf/releases




1. 可以在无需重新部署程序的情况下更新数据结构（向前兼容，向后兼容）
2. 支持的字段操作符：required， optional， repeated
3. 同一个proto文件，可以被protobuf编译器编译成特定语言。
4. 修改字段的注意事项：

   - 不可以改变已经存在的标签的数字。
   - 不可以增加或删除必须(required)字段。
   - 可以删除可选(optional)或重复(repeated)字段。
   - 可以添加新的可选或重复字段，但是必须使用新的标签数字，必须是之前的字段所没有用过的。 
  
注意：

    1. 需要注意的是新的可选消息不会在旧的消息中显示，所以你需要使用 has 严格的检查他们是否存在
    2. 或者在 .proto 文件中提供一个缺省值。如果没有缺省值，就会有一个类型相关的默认缺省值：对于字符串就是空字符串；对于布尔型则是false；对于数字类型默认为0。
    3. 其他：如果你添加了新的重复字段，你的新代码不会告诉你这个字段为空(新代码)也不会，也不会(旧代码)包含 has 标志。

参考资料
---------------------------
*  tutorial: https://developers.google.com/protocol-buffers/docs/tutorials

* Documentation: https://developers.google.com/protocol-buffers/