static int32_t qlearner_type() {
	return DoraType<MLQLearner>();
}
static void mlqlearner_update(int64_t self, int64_t state, int32_t action, double reward) {
	r_cast<MLQLearner*>(self)->update(s_cast<MLQState>(state), s_cast<MLQAction>(action), reward);
}
static int32_t mlqlearner_get_best_action(int64_t self, int64_t state) {
	return s_cast<int32_t>(r_cast<MLQLearner*>(self)->getBestAction(s_cast<MLQState>(state)));
}
static void mlqlearner_visit_matrix(int64_t self, int32_t func, int64_t stack) {
	std::shared_ptr<void> deref(nullptr, [func](auto) {
		SharedWasmRuntime.deref(func);
	});
	auto args = r_cast<CallStack*>(stack);
	ml_qlearner_visit_state_action_q(r_cast<MLQLearner*>(self), [func, args, deref](MLQState state, MLQAction action, double q) {
		args->clear();
		args->push(s_cast<int64_t>(state));
		args->push(s_cast<int64_t>(action));
		args->push(q);
		SharedWasmRuntime.invoke(func);
	});
}
static int64_t mlqlearner_pack(int64_t hints, int64_t values) {
	return s_cast<int64_t>(MLQLearner::pack(from_uint32_vec(hints), from_uint32_vec(values)));
}
static int64_t mlqlearner_unpack(int64_t hints, int64_t state) {
	return to_vec(MLQLearner::unpack(from_uint32_vec(hints), s_cast<MLQState>(state)));
}
static int64_t mlqlearner_new(double gamma, double alpha, double max_q) {
	return from_object(MLQLearner::create(gamma, alpha, max_q));
}
static void linkMLQLearner(wasm3::module& mod) {
	mod.link_optional("*", "qlearner_type", qlearner_type);
	mod.link_optional("*", "mlqlearner_update", mlqlearner_update);
	mod.link_optional("*", "mlqlearner_get_best_action", mlqlearner_get_best_action);
	mod.link_optional("*", "mlqlearner_visit_matrix", mlqlearner_visit_matrix);
	mod.link_optional("*", "mlqlearner_pack", mlqlearner_pack);
	mod.link_optional("*", "mlqlearner_unpack", mlqlearner_unpack);
	mod.link_optional("*", "mlqlearner_new", mlqlearner_new);
}