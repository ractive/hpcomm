// CalcObj.cpp: implementation of the CCalcObj class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CalcTypes.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCalcTypes *CCalcTypes::instance_ = 0;

// Do *not* change these messages, as they are what the calculator returns.
// If the messages are changed, HPComm will not recognise the object types.

// If a particular object doesn't have a localised string, put a 0 in.

CCalcType CCalcTypes::msg_[] = {
//    +----------------------- Icon number (must match .bmp files)
//    |  +-------------------- English type
//    |  |         +---------- French type
//    |  |         |        +- Spanish type
//    |  |         |        |
    { 0, "Unknown", 0, 0}, // SPECIAL
    { 1, "Real Number", "Nombre Réel", "Número real"},
    { 2, "Complex Number", "Nombre Complexe", "Número complejo"},
    { 3, "String", "Chaîne", "Cadena"},
    { 4, "Real Array", "Tableau de Réels", "Forma.núms.reales"},
    { 5, "Complex Array", "Tableau Complexe", "Forma.núms.complej"},
    { 6, "List", "Liste", "Lista"}, // HP38G "F" and ("E" if varname matches "L*").
    { 7, "Global Name", "Nom Global", 0},
    { 8, "Local Name", "Nom local", 0}, 
    { 9, "Program", "Programme", 0}, // HP38G "D"
    {10, "Algebraic", "Expression Algébrique", 0}, 
    {11, "Binary Integer", "Entier Binaire", 0}, 
    {12, "Graphic", "Graphique", 0}, 
    {13, "Tagged", "Identifié(s)", 0}, 
    {14, "Unit", "Unité", 0}, 
    {15, "XLIB Name", "Nom de XLIB", 0}, 
    {16, "Directory", "Répertoire", 0}, // HP38G "A"
    {17, "Open Directory", 0, 0}, // SPECIAL
    {18, "Unvisited Directory", 0, 0}, // SPECIAL
    {19, "Library", "Bibliothèque", 0}, // HP38G "H"
    {20, "Backup", "Sauvegarde", 0}, 
    {21, "Function", "Fonction", 0}, 
    {22, "Command", "Commande", 0}, 
    {23, "System Binary", "Entier Système", "Val.bin.de sistema"},
    {24, "Long Real", "Réel Long", "Número real largo"},
    {25, "Long Complex", "Complexe Long", "Núm.complejo largo"},
    {26, "Linked Array", "Tableau Indexé", "Formación encaden."},
    {27, "Character", "Caractère", "Carácter"},
    {28, "Code", 0, "Cóodigo"},
    {29, "Library Data", "Données Biblioth.", "Datos, biblioteca"},
    {30, "External", "Externe", "Externo"},
    {31, "Integer", "Entier", 0}, 
    {32, "Symbolic Matrix", "Matrice Symbolique", 0}, 
    {33, "Font", "Fonte", 0}, 
    {34, "Aplet", 0, 0}, // HP38G "B"
    {35, "Extended Real", "Reel Etendu", 0}, 
    {36, "Extended Complex", "Complexe Etendu", 0}, 
    {37, "FlashPtr", "Pointeur Flash", 0}, 
    {38, "Extended Ptr", "Pointeur Etendu", 0}, 
    {39, "MiniFont", "MiniFonte", 0}, 
    {40, "Extended 1", 0, 0}, 
    {41, "Extended 2", 0, 0}, 
    {42, "Extended 3", 0, 0}, 
    
    {49, "HOME", 0, 0}, // SPECIAL the icon for the root directory.
    
    // These entries are exclusively for the HP38G.
    {50, "Note", 0, 0}, // HP38G "C"
    {51, "Variable", 0, 0}, // HP38G "E"
    {52, "Matrix", 0, 0}, // HP38G "G" and ("E" if varname matches "M*").
    {53, "Target List", 0, 0}, // HP38G "I"
    {54, "Vector", 0, 0}, // HP38G "J"
    {55, "Wildcard", 0, 0}, // HP38G "K"
};

int CCalcTypes::msgs_ = sizeof(msg_) / sizeof(CCalcType);

CCalcTypes::
CCalcTypes()
{
#if 0
    // Initialise the hits array
    for (int i = 0; i < CCalcType::langs_; i++)
        hits_[i] = 0;
#endif
    // No cached item.
    mra_ = 0;
    // Default language is English.
    mrl_ = 0;

    // Substitute the English value for any unspecified types.
    for (int lang = 1; lang < CCalcType::langs_; lang++)
    {
        for (int i = 0; i < msgs_; i++) {
            if (msg_[i].msg_[lang] == 0)
                msg_[i].msg_[lang] = msg_[i].msg_[0];
        }
    }

    directory_ = find("Directory");
    directoryIcon_ = directory_->iconIndex_;
    openDirectoryIcon_ = find("Open Directory")->iconIndex_;
}


CCalcTypes *CCalcTypes::
instance()
{
    if (instance_ == 0)
        instance_ = new CCalcTypes;

    return instance_;
}


#if 0
const char *CCalcTypes::
lookup(const char *key)
{
    return 0;
}
#endif


bool CCalcTypes::
isA(const char *key, const char *candidate)
{
    // This might spoil the cached key.  Should we save it then restore it later?
    CCalcType *keyp = find(key);
    CCalcType *candp = find(candidate);
    if (candp == 0)
        return false;
    if (candp != keyp)
        return false;
    
    return true;
}


bool CCalcTypes::
known(const char *key)
{
    CCalcType *cm = find(key);
    
    return cm != 0;
}


CCalcType *CCalcTypes::
find(const char *key)
{
    // Is is the most recent type?
    if (mra_ != 0) {
        if (strcmp(key, mra_->msg_[mrl_]) == 0)
            return mra_;
    }
    
    // Can we find it in the current language?
    for (int i = 0; i < msgs_; i++) {
        if (strcmp(key, msg_[i].msg_[mrl_]) == 0) {
            mra_ = &msg_[i];
            return mra_;
        }
    }
    
    // No, not in the current language, so check the others.
    for (int lang = 0; lang < CCalcType::langs_; lang++)
    {
        // Don't check the language we just checked.
        if (lang == mrl_)
            continue;
        for (int i = 0; i < msgs_; i++) {
            if (strcmp(key, msg_[i].msg_[lang]) == 0) {
                mrl_ = lang;
                mra_ = &msg_[i];
                return mra_;
            }
        }
    }
    
    return 0;
}


const int CCalcTypes::
icon(const char *key)
{
    CCalcType *cm = find(key);

    if (cm == 0)
        return 0;

    return cm->iconIndex_;
}


bool CCalcTypes::
isADirectory(const char *candidate)
{
    for (int lang = 0; lang < CCalcType::langs_; lang++)
    {
        if (strcmp(candidate, directory_->msg_[lang]) == 0) {
            return true;
        }
    }
        
    return false;
}


const int CCalcTypes::
directoryIcon()
{
    return directoryIcon_;
}


const int CCalcTypes::
openDirectoryIcon()
{
    return openDirectoryIcon_;
}

void CCalcTypes::Destroy()
{
    if (instance_ != 0) {
        delete instance_;
		instance_ = 0;
	}
}
