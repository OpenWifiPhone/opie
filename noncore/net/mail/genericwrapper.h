#ifndef __GENERIC_WRAPPER_H
#define __GENERIC_WRAPPER_H

#include "abstractmail.h"
#include <qmap.h>
#include <qstring.h>
#include <libetpan/clist.h>

class RecMail;
class RecBody;
class encodedString;
struct mailpop3;
struct mailmessage;
struct mailmime;
struct mailmime_mechanism;
struct mailimf_mailbox_list;
struct mailimf_mailbox;
struct mailimf_date_time;
struct mailimf_group;
struct mailimf_address_list;

/* this class hold just the funs shared between
 * mbox and pop3 (later mh, too) mail access.
 * it is not desigend to make a instance of it!
 */
class Genericwrapper : public AbstractMail
{
    Q_OBJECT
public:
    Genericwrapper();
    virtual ~Genericwrapper();

    virtual encodedString* fetchDecodedPart(const RecMail&mail,const RecPart&part);
    virtual encodedString* fetchRawPart(const RecMail&mail,const RecPart&part);
    virtual QString fetchTextPart(const RecMail&mail,const RecPart&part);
    virtual void cleanMimeCache();

protected:
    RecMail *parseHeader( const char *header );
    RecBody parseMail( mailmessage * msg );
    QString parseMailboxList( mailimf_mailbox_list *list );
    QString parseMailbox( mailimf_mailbox *box );
    QString parseGroup( mailimf_group *group );
    QString parseAddressList( mailimf_address_list *list );
    QString parseDateTime( mailimf_date_time *date );
    
    void traverseBody(RecBody&target,mailmessage*message,mailmime*mime,unsigned int current_rek=0);
    static void fillSingleBody(RecPart&target,mailmessage*message,mailmime*mime);
    static void fillParameters(RecPart&target,clist*parameters);
    static QString getencoding(mailmime_mechanism*aEnc);

    QString msgTempName;
    unsigned int last_msg_id;
    QMap<QString,encodedString*> bodyCache;
};

#endif
