#include <Shared/Core/Assert.h>

#include <Shared/Core/grimLmdb.h>

namespace grimLmdb
{
	Environment::Environment():
		m_environment(lmdb::env::create())
	{}

	Environment::~Environment()
	{
		Close();
	}

	void Environment::Open(const char* filepath)
	{
		m_environment.open(filepath, MDB_NOSUBDIR, 0664);
	}
	
	void Environment::Close()
	{
		m_environment.close();
	}
}