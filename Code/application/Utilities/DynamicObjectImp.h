/*
 * The information in this file is
 * Copyright(c) 2007 Ball Aerospace & Technologies Corporation
 * and is subject to the terms and conditions of the
 * GNU Lesser General Public License Version 2.1
 * The license text is available from   
 * http://www.gnu.org/licenses/lgpl.html
 */

#ifndef __DYNMCOBJIMP_H
#define __DYNMCOBJIMP_H

#include "DataVariant.h"
#include "SerializableImp.h"
#include "SubjectImp.h"
#include "xmlreader.h"
#include "xmlwriter.h"

#include <map>
#include <string>
#include <vector>

#include <QtCore/QStringList>

class DynamicObject;

class DynamicObjectImp : public SubjectImp, public Serializable
{
public:
   DynamicObjectImp();
   virtual ~DynamicObjectImp();

   virtual DynamicObjectImp& operator= (const DynamicObjectImp& rhs);

   virtual void merge(const DynamicObject* pObject);
   virtual bool setAttribute(const std::string& name, const DataVariant &value);
   virtual bool adoptAttribute(const std::string& name, DataVariant &value);
   virtual bool setAttributeByPath(QStringList pathComponents, const DataVariant &value);
   virtual bool adoptAttributeByPath(QStringList pathComponents, DataVariant &value);
   virtual bool setAttributeByPath(const std::string& path, const DataVariant &value);
   virtual bool adoptAttributeByPath(const std::string& path, DataVariant &value);
   virtual bool setAttributeByPath(const std::string pComponents[], const DataVariant& value);
   virtual bool adoptAttributeByPath(const std::string pComponents[], DataVariant &value);
   virtual const DataVariant &getAttribute(const std::string& name) const;
   virtual DataVariant &getAttribute(const std::string& name);
   virtual const DataVariant &getAttributeByPath(QStringList pathComponents) const;
   virtual DataVariant &getAttributeByPath(QStringList pathComponents);
   virtual const DataVariant &getAttributeByPath(const std::string &path) const;
   virtual DataVariant &getAttributeByPath(const std::string &path);
   virtual const DataVariant &getAttributeByPath(const std::string pComponents[]) const;
   virtual DataVariant &getAttributeByPath(const std::string pComponents[]);
   virtual void getAttributeNames(std::vector<std::string>& attributeNames) const;
   virtual unsigned int getNumAttributes() const;
   virtual bool removeAttribute(const std::string& name);
   virtual bool removeAttributeByPath(const std::string pComponents[]);
   virtual bool removeAttributeByPath(const std::string& path);
   virtual bool removeAttributeByPath(QStringList pathComponents);
   virtual void clear();

   const std::string& getObjectType() const;
   bool isKindOf(const std::string& className) const;
   virtual bool toXml(XMLWriter* pXml) const;
   virtual bool fromXml(DOMNode* pDocument, unsigned int version);

protected:
   virtual bool setAttribute(const std::string& name, DataVariant &value, bool swap);
   virtual bool setAttributeByPath(const std::string& name, DataVariant &value, bool swap);
   virtual bool setAttributeByPath(const std::string pComponents[], DataVariant& value, bool swap);
   virtual bool setAttributeByPath(QStringList pathComponents, DataVariant &value, bool swap);

   /**
   * Determines if the parameter is a child (or grandchild, or the same instance)
   * @param pObject
   *        Potential child
   * @return
   *        true if this object is a parent (or grandparent, etc) of pObject
   */
   virtual bool isParentOf(const DynamicObjectImp *pObject) const;

   std::map<std::string, DataVariant> mVariantAttributes;
};

#define DYNAMICOBJECTADAPTER_METHODS(impClass) \
   SUBJECTADAPTER_METHODS(impClass) \
   SERIALIZABLEADAPTER_METHODS(impClass) \
   void merge(const DynamicObject* pObject) \
   { \
      return impClass::merge(pObject); \
   } \
   bool setAttribute(const std::string& name, const DataVariant& data) \
   { \
      return impClass::setAttribute(name, data); \
   } \
   bool adoptAttribute(const std::string& name, DataVariant& data) \
   { \
      return impClass::adoptAttribute(name, data); \
   } \
   bool setAttributeByPath(const std::string& path, const DataVariant &value) \
   { \
      return impClass::setAttributeByPath(path, value); \
   } \
   bool adoptAttributeByPath(const std::string& path, DataVariant &value) \
   { \
      return impClass::adoptAttributeByPath(path, value); \
   } \
   bool setAttributeByPath(const std::string pComponents[], const DataVariant &value) \
   { \
      return impClass::setAttributeByPath(pComponents, value); \
   } \
   bool adoptAttributeByPath(const std::string pComponents[], DataVariant &value) \
   { \
      return impClass::adoptAttributeByPath(pComponents, value); \
   } \
   bool setAttributeByPath(QStringList pathComponents, const DataVariant &value) \
   { \
      return impClass::setAttributeByPath(pathComponents, value); \
   } \
   bool adoptAttributeByPath(QStringList pathComponents, DataVariant &value) \
   { \
      return impClass::adoptAttributeByPath(pathComponents, value); \
   } \
   DataVariant &getAttribute(const std::string& name) \
   { \
      return impClass::getAttribute(name); \
   } \
   const DataVariant &getAttribute(const std::string& name) const \
   { \
      return impClass::getAttribute(name); \
   } \
   const DataVariant &getAttributeByPath(const std::string &path) const \
   { \
      return impClass::getAttributeByPath(path); \
   } \
   DataVariant &getAttributeByPath(const std::string pComponents[]) \
   { \
      return impClass::getAttributeByPath(pComponents); \
   } \
   const DataVariant &getAttributeByPath(const std::string pComponents[]) const \
   { \
      return impClass::getAttributeByPath(pComponents); \
   } \
   DataVariant &getAttributeByPath(const std::string &path) \
   { \
      return impClass::getAttributeByPath(path); \
   } \
   void getAttributeNames(std::vector<std::string>& attributeNames) const \
   { \
      return impClass::getAttributeNames(attributeNames); \
   } \
   unsigned int getNumAttributes() const \
   { \
      return impClass::getNumAttributes(); \
   } \
   bool removeAttribute(const std::string& name) \
   { \
      return impClass::removeAttribute(name); \
   } \
   bool removeAttributeByPath(const std::string pComponents[]) \
   { \
      return impClass::removeAttributeByPath(pComponents); \
   } \
   bool removeAttributeByPath(const std::string& path) \
   { \
      return impClass::removeAttributeByPath(path); \
   } \
   void clear() \
   { \
      return impClass::clear(); \
   }

#endif   // __DYNMCOBJIMP_H