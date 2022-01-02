/*
 * IConfig.h
 *
 *  Created on: 23 sept. 2021
 *      Author: azeddine
 */

#ifndef ICODE_INCLUDE_RUNTIME_ICONFIG_H_
#define ICODE_INCLUDE_RUNTIME_ICONFIG_H_
#include "../core/core.h"
class IConfig;
class Config;
class ConfigEvent;
typedef void (*ConfigNotify)(ConfigEvent &e);
typedef void (*ConfigEnumeration)(Config &parent, void *userdata,
		const char *child);
struct ConfigLintener {
	ConfigNotify listener;
	void *userdata;
	int flags;
};
class Config {
public:
	IConfig *config;
	inline static int _AddRef(IConfig *config);
	inline static int _Release(IConfig *config);
	Config() {
		config = 0;
	}
	Config(IConfig *config) {
		if (config != 0)
			_AddRef(config);
		this->config = config;
	}
	Config(const Config &lp) {
		if (lp.config != 0)
			_AddRef(lp.config);
		this->config = lp.config;
	}
	~Config() {
		if (this->config != 0)
			_Release(this->config);
	}
	void operator=(IConfig *config) {
		if (config != 0)
			_AddRef(config);
		if (this->config != 0)
			_Release(this->config);
		this->config = config;
	}
	void operator=(const Config &lp) {
		if (lp.config != 0)
			_AddRef(lp.config);
		if (this->config != 0)
			_Release(this->config);
		this->config = lp.config;
	}
	bool IsOk() {
		return this->config != 0;
	}
	bool operator==(const Config &lp) {
		return this->config == lp.config;
	}
	bool operator==(IConfig *obj) {
		return this->config != obj;
	}
	bool operator!=(const Config &lp) {
		return this->config == lp.config;
	}
	bool operator!=(IConfig *obj) {
		return this->config != obj;
	}
public:
	inline int _GetAttribute();
	inline int _GetSize();
	inline int _Delete();
	inline bool _AddListenner(ConfigLintener *listener);
	inline int _GetListennerLength();
	inline bool _GetListenner(int index, ConfigLintener *listener);
	inline bool _RemoveListenner(ConfigLintener *listener);
	inline const char* _GetName();
	inline int _GetValue(int type, void *data, size_t size);
	inline int _SetValue(int type, const void *data, size_t size);
	inline int _Open(const char *name, int attr, Config &config);
	inline int _ForAll(ConfigEnumeration notify, void *userdata);
public:
	enum {
		/* types file flags */
		TYPE_MASK = 0x07, //
		INT = 0, //
		BOOL = 1, //
		LONG = 2, //
		FLOAT = 3, //
		DOUBLE = 4, //
		STRING = 5, //
		BIN = 6, //
		DIR = 7, //
		/* open flags */
		CREATE = 1 << 4, //
	};
	int GetAttribute() {
		if (this->config != 0) {
			return _GetAttribute();
		} else
			return 0;
	}
	int GetSize() {
		if (this->config != 0) {
			return _GetSize();
		} else
			return 0;
	}
	int Delete() {
		if (this->config != 0) {
			return _Delete();
		} else
			return 0;
	}
	bool AddListenner(ConfigLintener *listener) {
		if (this->config != 0) {
			return _AddListenner(listener);
		} else
			return false;
	}
	int GetListennerLength() {
		if (this->config != 0) {
			return _GetListennerLength();
		} else
			return 0;
	}
	bool GetListenner(int index, ConfigLintener *listener) {
		if (this->config != 0) {
			return _GetListenner(index, listener);
		} else
			return 0;
	}
	bool RemoveListenner(ConfigLintener *listener) {
		if (this->config != 0) {
			return _RemoveListenner(listener);
		} else
			return 0;
	}
	const char* GetName() {
		if (this->config != 0) {
			return _GetName();
		} else
			return 0;
	}
	int GetValue(int type, void *data, size_t size) {
		if (this->config != 0) {
			return _GetValue(type, data, size);
		} else
			return 0;
	}
	int SetValue(int type, const void *data, size_t size) {
		if (this->config != 0) {
			return _SetValue(type, data, size);
		} else
			return 0;
	}
	int ForAll(ConfigEnumeration notify, void *userdata) {
		if (this->config != 0) {
			return _ForAll(notify, userdata);
		} else {
			return 0;
		}
	}
	Config& Open(const char *name, int attr, Config &config) {
		if (this->config != 0) {
			_Open(name, attr, config);
		}
		return config;
	}
	Config Open(const char *name, int attr) {
		Config config;
		if (this->config != 0) {
			_Open(name, attr, config);
		}
		return config;
	}
	Config& Open(const char *name, Config &config) {
		return Open(name, 0, config);
	}
	Config Open(const char *name) {
		return Open(name, 0);
	}
	Config& Create(const char *name, int type, Config &config) {
		return Open(name, type | Config::CREATE, config);
	}
	Config Create(const char *name, int type) {
		return Open(name, type | Config::CREATE);
	}
	Config& CreateInt(const char *name, Config &config) {
		return Open(name, Config::INT | Config::CREATE, config);
	}
	Config CreateInt(const char *name) {
		return Open(name, Config::INT | Config::CREATE);
	}
	Config& CreateBool(const char *name, Config &config) {
		return Open(name, Config::BOOL | Config::CREATE, config);
	}
	Config CreateBool(const char *name) {
		return Open(name, Config::BOOL | Config::CREATE);
	}
	Config& CreateFloat(const char *name, Config &config) {
		return Open(name, Config::FLOAT | Config::CREATE, config);
	}
	Config CreateFloat(const char *name) {
		return Open(name, Config::FLOAT | Config::CREATE);
	}
	Config& CreateDouble(const char *name, Config &config) {
		return Open(name, Config::DOUBLE | Config::CREATE, config);
	}
	Config CreateDouble(const char *name) {
		return Open(name, Config::DOUBLE | Config::CREATE);
	}
	Config& CreateString(const char *name, Config &config) {
		return Open(name, Config::STRING | Config::CREATE, config);
	}
	Config CreateString(const char *name) {
		return Open(name, Config::STRING | Config::CREATE);
	}
	Config& CreateBin(const char *name, Config &config) {
		return Open(name, Config::BIN | Config::CREATE, config);
	}
	Config CreateBin(const char *name) {
		return Open(name, Config::BIN | Config::CREATE);
	}
	Config& CreateDir(const char *name, Config &config) {
		return Open(name, Config::DIR | Config::CREATE, config);
	}
	Config CreateDir(const char *name) {
		return Open(name, Config::DIR | Config::CREATE);
	}
public:
	bool IsDir() {
		if (this->config != 0) {
			return (_GetAttribute() & TYPE_MASK) == DIR;
		} else
			return false;
	}
	bool IsInt() {
		if (this->config != 0) {
			return (_GetAttribute() & TYPE_MASK) == INT;
		} else
			return false;
	}
	bool SetInt(int value) {
		return SetValue(INT, &value, sizeof(int)) > 0;
	}
	bool GetInt(int *value) {
		return GetValue(INT, value, sizeof(int)) > 0;
	}
	int GetInt(int defValue) {
		GetInt(&defValue);
		return defValue;
	}
	bool IsBool() {
		if (this->config != 0) {
			return (_GetAttribute() & TYPE_MASK) == BOOL;
		} else
			return false;
	}
	bool SetBool(bool value) {
		return SetValue(BOOL, &value, sizeof(bool)) > 0;
	}
	bool GetBool(bool *value) {
		return GetValue(BOOL, value, sizeof(bool)) > 0;
	}
	bool GetBool(bool defValue) {
		GetBool(&defValue);
		return defValue;
	}
	bool IsFloat() {
		if (this->config != 0) {
			return (_GetAttribute() & TYPE_MASK) == FLOAT;
		} else
			return false;
	}
	bool SetFloat(float value) {
		return SetValue(FLOAT, &value, sizeof(float)) > 0;
	}
	bool GetFloat(float *value) {
		return GetValue(FLOAT, value, sizeof(float)) > 0;
	}
	float GetFloat(float defValue) {
		GetFloat(&defValue);
		return defValue;
	}
	bool IsDouble() {
		if (this->config != 0) {
			return (_GetAttribute() & TYPE_MASK) == DOUBLE;
		} else
			return false;
	}
	bool SetDouble(double value) {
		return SetValue(DOUBLE, &value, sizeof(double)) > 0;
	}
	bool GetDouble(double *value) {
		return GetValue(DOUBLE, value, sizeof(double)) > 0;
	}
	double GetDouble(double defValue) {
		GetDouble(&defValue);
		return defValue;
	}
	bool IsString() {
		if (this->config != 0) {
			return (_GetAttribute() & TYPE_MASK) == STRING;
		} else
			return false;
	}
	bool SetString(const char *str, size_t length) {
		return SetValue(STRING, str, length) > 0;
	}
	bool SetString(const char *str) {
		return SetValue(STRING, str, -1) > 0;
	}
	bool GetString(char *str, size_t length) {
		return GetValue(STRING, str, length) > 0;
	}
	char* GetString() {
		if (this->config != 0) {
			if ((_GetAttribute() & TYPE_MASK) == STRING) {
				int size = _GetSize();
				char *s =(char *) malloc(size + 1);
				if (s != 0) {
					_GetValue(STRING, s, size);
					s[size] = 0;
					return s;
				}
			}
		}
		return 0;
	}
	bool IsBin() {
		if (this->config != 0) {
			return (_GetAttribute() & TYPE_MASK) == BIN;
		} else
			return false;
	}
	bool SetBin(const void *ptr, size_t length) {
		return SetValue(BIN, ptr, length) > 0;
	}
	bool GetBin(void *ptr, size_t length) {
		return GetValue(BIN, ptr, length) > 0;
	}
	/*
	 *
	 */
public:
	static void PrintNotify(Config &parent, void *userdata, const char *child) {
		puts(child);
		puts("\n");
	}
	void PrintChildren() {
		ForAll(PrintNotify, 0);
	}
};
struct ConfigEvent {
	enum {
		CONFIG_CREATED = 1, CONFIG_UPDATED, CONFIG_DELETED
	};
	int type;
	Config configUpdated;
	Config config;
	void *userdata;
};
class IConfig: public IObject {
protected:
	~IConfig() {

	}
public:
	virtual int GetAttribute() = 0;
	virtual int GetSize() = 0;
	virtual int Delete()=0;
	virtual bool AddListenner(ConfigLintener *listener) = 0;
	virtual int GetListennerLength() = 0;
	virtual bool GetListenner(int index, ConfigLintener *listener) = 0;
	virtual bool RemoveListenner(ConfigLintener *listener) = 0;
	virtual const char* GetName() = 0;
	virtual int GetValue(int type, void *data, size_t size)=0;
	virtual int SetValue(int type, const void *data, size_t size)=0;
	virtual int Open(const char *name, int attr, Config &config)=0;
	virtual int ForAll(ConfigEnumeration notify, void *userdata)=0;
};
inline int Config::_AddRef(IConfig *config) {
	return config->AddRef();
}

inline int Config::_Release(IConfig *config) {
	return config->Release();
}

inline int Config::_GetAttribute() {
	return this->config->GetAttribute();
}

inline int Config::_GetSize() {
	return this->config->GetSize();
}

inline int Config::_Delete() {
	return this->config->Delete();
}

inline bool Config::_AddListenner(ConfigLintener *listener) {
	return this->config->AddListenner(listener);
}

inline int Config::_GetListennerLength() {
	return this->config->GetListennerLength();
}

inline bool Config::_GetListenner(int index, ConfigLintener *listener) {
	return this->config->GetListenner(index, listener);
}

inline bool Config::_RemoveListenner(ConfigLintener *listener) {
	return this->config->RemoveListenner(listener);
}

inline const char* Config::_GetName() {
	return this->config->GetName();
}

inline int Config::_GetValue(int type, void *data, size_t size) {
	return this->config->GetValue(type, data, size);
}

inline int Config::_SetValue(int type, const void *data, size_t size) {
	return this->config->SetValue(type, data, size);
}

inline int Config::_Open(const char *name, int attr, Config &config) {
	return this->config->Open(name, attr, config);
}
inline int Config::_ForAll(ConfigEnumeration notify, void *userdata) {
	return this->config->ForAll(notify, userdata);
}

#endif /* ICODE_INCLUDE_RUNTIME_ICONFIG_H_ */
