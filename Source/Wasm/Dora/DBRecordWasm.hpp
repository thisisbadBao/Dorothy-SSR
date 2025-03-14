static void dbrecord_release(int64_t raw) {
	delete r_cast<DBRecord*>(raw);
}
static int32_t dbrecord_read(int64_t self, int64_t record) {
	return r_cast<DBRecord*>(self)->read(r_cast<Array*>(record)) ? 1 : 0;
}
static void linkDBRecord(wasm3::module& mod) {
	mod.link_optional("*", "dbrecord_release", dbrecord_release);
	mod.link_optional("*", "dbrecord_read", dbrecord_read);
}