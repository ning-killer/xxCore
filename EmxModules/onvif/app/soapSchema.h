#pragma once
#include <string>
#include <vector>

enum tt__PropertyOperation {
    tt__PropertyOperation__Initialized = 0,
    tt__PropertyOperation__Deleted = 1,
    tt__PropertyOperation__Changed = 2
};

struct wsa5__ReferenceParametersType
{
    /** Optional element 'chan:ChannelInstance' of XSD type 'xsd:int' */
    int chan__ChannelInstance;
    /** Sequence of elements '-any' of XSD type 'xsd:anyType' stored in dynamic array __any of length __size */
    int __size;
    char** __any;
    /** Optional attribute '-anyAttribute' of XSD type 'xsd:anyType' */
    char* __anyAttribute;
};
struct wsa5__MetadataType
{
    /** Sequence of elements '-any' of XSD type 'xsd:anyType' stored in dynamic array __any of length __size */
    int __size;
    char** __any;
    /** Optional attribute '-anyAttribute' of XSD type 'xsd:anyType' */
    char* __anyAttribute;
};
class wsa5__EndpointReferenceType
{
public:
    /** Required element 'wsa5:Address' of XSD type 'xsd:string' */
    std::string Address;
    /** Optional element 'wsa5:ReferenceParameters' of XSD type 'wsa5:ReferenceParametersType' */
    std::shared_ptr<wsa5__ReferenceParametersType> ReferenceParameters;
    /** Optional element 'wsa5:Metadata' of XSD type 'wsa5:MetadataType' */
    std::shared_ptr<wsa5__MetadataType> Metadata;
    /** Sequence of elements '-any' of XSD type 'xsd:anyType' stored in dynamic array __any of length __size */
    int __size;
    char** __any;
    /** Optional attribute '-anyAttribute' of XSD type 'xsd:anyType' */
    char* __anyAttribute;
};
class wsnt__TopicExpressionType
{
public:
    std::string v;
   // struct soap_dom_element __any;
    /// required attribute 'Dialect' of XSD type 'xsd:anyURI'
    std::string Dialect;
};
class tt__Vector
{
public:
    float x;// optional attribute 'x'
    float y;// optional attribute 'y'
};
class tt__Transformation
{
public:
    tt__Vector Translate;// Optional element 'tt:Translate'
    tt__Vector Scale;// Optional element 'tt:Scale'
};
class tt__CellLayout
{
public:
    tt__Transformation Transformation; // Required element 'tt:Transformation'
    std::string Columns;// required attribute 'Columns'
    std::string Rows;// required attribute 'Rows'
};
class _tt__ItemList_SimpleItem
{
public:
    std::string Name;// required attribute 'Name' 
    std::string Value;// required attribute 'Value'
};
class wsnt__FilterType
{
public:
   // std::string Dialect;
    wsnt__TopicExpressionType TopicExpression;
};
class _tt__ItemList_ElementItem
{
public:
    tt__CellLayout Layout;
    // XML DOM element node graph
    //struct soap_dom_element __any;
    std::string Name;// required attribute 'Name'
};
class tt__ItemList
{
public:
    // Optional element 'tt:SimpleItem' of XSD type 'tt:ItemList-SimpleItem'
    std::vector<_tt__ItemList_SimpleItem> SimpleItem;
    /// Optional element 'tt:ElementItem' of XSD type 'tt:ItemList-ElementItem'
    std::vector<_tt__ItemList_ElementItem> ElementItem;
    /// Optional element 'tt:Extension' of XSD type 'tt:ItemListExtension'
    //tt__ItemListExtension* Extension;
};
class _tt__Message
{
public:
    std::shared_ptr<tt__ItemList> Source; // Optional element 'tt:Source'
    std::shared_ptr<tt__ItemList> Key;// Optional element 'tt:Key' 
    std::shared_ptr<tt__ItemList> Data;// Optional element 'tt:Data' 
    //tt__MessageExtension* Extension;// Optional element 'tt:Extension'
    time_t UtcTime;// required attribute 'UtcTime'
    std::shared_ptr<enum tt__PropertyOperation> PropertyOperation;// optional attribute 'PropertyOperation'
};
class _wsnt__NotificationMessageHolderType_Message
{
public:
    _tt__Message tt_Message;
    //struct soap_dom_element __any;
};
class wsnt__NotificationMessageHolderType
{
public:
    std::shared_ptr<wsa5__EndpointReferenceType> SubscriptionReference;// Optional element 'wsnt:SubscriptionReference'
    std::shared_ptr<wsnt__TopicExpressionType> Topic;// Optional element 'wsnt:Topic'
    std::shared_ptr<wsa5__EndpointReferenceType> ProducerReference;// Optional element 'wsnt:ProducerReference'
    _wsnt__NotificationMessageHolderType_Message Message;// Required element 'wsnt:Message'
};
class _tt__ItemListDescription_SimpleItemDescription
{
public:
    std::string Name;// required attribute 'Name'
    std::string Type;// required attribute
};
class _tt__ItemListDescription_ElementItemDescription
{
public: 
    std::string Name; // required attribute 'Name'
    std::string Type;// required attribute 'Type'
};
class tt__ItemListDescription
{
public:
    std::vector<_tt__ItemListDescription_SimpleItemDescription> SimpleItemDescription;// Optional element 'tt:SimpleItemDescription'
    std::vector<_tt__ItemListDescription_ElementItemDescription> ElementItemDescription;// Optional element 'tt:ElementItemDescription'
};
class tt__MessageDescription
{
public:
    std::shared_ptr<tt__ItemListDescription> Source;// Optional element 'tt:Source' 
    std::shared_ptr<tt__ItemListDescription> Key;// Optional element 'tt:Key' 
    std::shared_ptr<tt__ItemListDescription> Data;// Optional element 'tt:Data'
    //tt__MessageDescriptionExtension* Extension;// Optional element 'tt:Extension'
    bool IsProperty;// optional attribute 'IsProperty'
};