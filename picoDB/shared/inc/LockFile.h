#ifndef LOCKFILE_H_
#define LOCKFILE_H_

#include <unistd.h>
#include <fcntl.h>
#include <string>

class LockFile {

private:
	struct flock fl;
	int fd;
	std::string nombre;

public:
	LockFile ( const std::string nombre );
	~LockFile();

	std::string nombreArchivo() const;

	int tomarLockLectura();
	int tomarLockEscritura ();
	int liberarLock ();
	ssize_t escribir ( const void* buffer,const ssize_t buffsize ) const;
	ssize_t leer ( void* buffer,const ssize_t buffsize ) const;
	void setInit();
};

#endif /* LOCKFILE_H_ */
