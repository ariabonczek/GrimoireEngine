#pragma once

#include <Shared/lmdb/lmdb++.h>

namespace grimLmdb
{
	class Environment
	{
	public:
		Environment();
		~Environment();

		void Open(const char* filepath);
		void Close();
		
		template<typename T>
		bool WriteValue(const char* key, T value)
		{
			// Write to the database
			lmdb::txn writeTransaction = lmdb::txn::begin(m_environment);
			lmdb::dbi databaseHandle = lmdb::dbi::open(writeTransaction, nullptr);

			bool success = databaseHandle.put<T>(writeTransaction, key, value);
			writeTransaction.commit();

			GRIM_ASSERT(success, "Failed to write key-value pair to lmdb. Key: %s", key);

			return success;
		}

		template<typename T>
		bool WriteArray(const char* key, T* value, size_t length)
		{
			// Write to the database
			lmdb::txn writeTransaction = lmdb::txn::begin(m_environment);
			lmdb::dbi databaseHandle = lmdb::dbi::open(writeTransaction, nullptr);

			lmdb::val keyAsVal;
			keyAsVal.assign(key, strlen(key));
			lmdb::val val;
			val.assign(value, length * sizeof(T));

			bool success = databaseHandle.put(writeTransaction, keyAsVal, val);
			writeTransaction.commit();

			GRIM_ASSERT(success, "Failed to write key-value pair to lmdb. Key: %s", key);

			return success;
		}

		bool WriteBuffer(const char* key, void* buffer, size_t size)
		{
			// Write to the database
			lmdb::txn writeTransaction = lmdb::txn::begin(m_environment);
			lmdb::dbi databaseHandle = lmdb::dbi::open(writeTransaction, nullptr);

			lmdb::val keyAsVal;
			keyAsVal.assign(key, strlen(key));
			lmdb::val val;
			val.assign(buffer, size);

			bool success = databaseHandle.put(writeTransaction, keyAsVal, val);
			writeTransaction.commit();

			GRIM_ASSERT(success, "Failed to write key-value pair to lmdb. Key: %s", key);

			return success;
		}

		template<typename T>
		T* ReadValue(const char* key)
		{
			// Read from the database
			lmdb::txn readTransaction = lmdb::txn::begin(m_environment, nullptr, MDB_RDONLY);
			lmdb::dbi databaseHandle = lmdb::dbi::open(readTransaction, nullptr);
			
			lmdb::val keyAsVal{ key, strlen(key) };
			lmdb::val value;
			
			bool success = databaseHandle.get(readTransaction.handle(), keyAsVal, value);

			GRIM_ASSERT(success, "Failed to read key from lmdb. Key: %s", key);


			return (T*)value.data();
		}

	private:
		lmdb::env m_environment;
	};

	
}