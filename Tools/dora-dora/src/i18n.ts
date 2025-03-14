import i18n from 'i18next';
import { initReactI18next } from 'react-i18next';

i18n
	.use(initReactI18next)
	.init({
		resources: {
			en: {
				translation: {
					spine: {
						load: "failed to load {{file}}",
					},
					tree: {
						assets: "Assets",
					},
					alert: {
						assetLoad: "failed to load assets",
						platform: "Dorothy SSR is running on {{platform}}",
						getInfo: "failed to get basic info",
						builtin: "can not operate on built-in assets",
						read: "failed to read {{title}}",
						reloading: "please save before reloading assets",
						reloaded: "assets reloaded",
						save: "failed to save {{title}}",
						saveCurrent: "failed to save current file",
						deleteRoot: "can not delete root folder",
						deleted: "deleted {{title}}",
						delete: "failed to delete item",
						deleteNoTarget: "select a file tree node to delete",
						downloadWait: "wait for previous download to finish",
						downloadStart: "wait for download to start",
						downloadFailed: "failed to prepare download",
						renameSave: "please save all files before renaming",
						renameRoot: "can not rename root folder",
						renameFailed: "failed to rename item",
						renamed: "renamed {{oldFile}} to {{newFile}}",
						newFailed: "failed to create item",
						newNoTarget: "select a file tree node before creating new file",
						movingNoSave: "please save all files before moving",
						movingFailed: "failed to move {{from}} to folder {{to}}",
						moved: "{{from}} moved to folder {{to}}",
						stopped: "stopped running",
						stopNone: "nothing to stop",
						stopFailed: "failed to stop running",
						runNoTarget: "please select a file to run",
						run: "{{title}} is running",
						runFailed: "failed to run {{title}}"
					},
					popup: {
						goToFile: "Go to File",
						closingTab: "Closing Tab",
						closingNoSave: "close tab without saving",
						moving: "Moving Item",
						movingInfo: "move {{from}} to folder {{to}}",
					},
					upload: {
						text: "Click to select a folder or drag files to this area to upload.",
						hint: "Uploading will start automatically.",
						selectFile: "Select File",
						uploading: "Uploading",
						startUpload: "Start Upload",
						success: "Upload successfully.",
						failed: "Upload failed."
					},
					action: {
						"ok": "OK",
						"cancel": "Cancel",
						"confirm": "Confirm",
					},
					file: {
						new: "New File",
						newFolder: "New Folder",
						rename: "Rename",
						enterFile: "Enter a file name",
						enterFolder: "Enter a folder name",
						deleteFile: "deleting file {{name}}",
						deleteFolder: "deleting folder {{name}}",
						lua: "lightweight, high-level, multi-paradigm language",
						yuescript: "expressive, extremely concise language",
						teal: "a typed dialect of Lua",
						xml: "write game node trees in Xml format",
						markdown: "do some documentation",
						folder: "create a file folder",
						yarn: "a language for game dialogue",
						visualScript: "write game logic by connecting visual nodes together",
					},
					editor: {
						goToDefinition: "Go to Definition",
						require: "Import this module",
					},
					menu: {
						version: "Dorothy SSR version {{version}}",
						new: "New",
						delete: "Delete",
						rename: "Rename",
						upload: "Upload",
						download: "Download",
						save: "Save",
						saveAll: "Save All",
						close: "Close",
						closeAll: "Close All",
						closeOthers: "Close Others",
						run: "Run",
						runThis: "Run This",
						stop: "Stop",
						goToFile: "Go to File",
					}
				}
			},
			zh: {
				translation: {
					spine: {
						load: "加载 {{file}} 失败",
					},
					tree: {
						assets: "资源根目录",
					},
					alert: {
						assetLoad: "资源加载失败",
						platform: "Dorothy SSR 正运行于 {{platform}} 平台",
						getInfo: "获取基本信息失败",
						builtin: "无法操作内置资源",
						read: "读取 {{title}} 失败",
						reloading: "请在重新加载资源之前保存",
						reloaded: "资源已重新加载",
						save: "保存 {{title}} 失败",
						saveCurrent: "保存当前文件失败",
						deleteRoot: "无法删除资源的根目录",
						deleted: "{{title}} 已删除",
						delete: "删除文件失败",
						deleteNoTarget: "请选择要删除的文件树节点",
						downloadWait: "请等待上一个下载完成",
						downloadStart: "请等待下载开始",
						downloadFailed: "下载准备失败",
						renameSave: "请在重命名之前保存所有文件",
						renameRoot: "无法重命名根目录文件夹",
						renameFailed: "重命名文件失败",
						renamed: "{{oldFile}} 已重命名为 {{newFile}}",
						newFailed: "创建文件失败",
						newNoTarget: "请选择要创建新文件的文件树节点",
						movingNoSave: "请在移动文件之前保存所有文件",
						movingFailed: "移动 {{from}} 到文件夹 {{to}} 失败",
						moved: "已移动 {{from}} 到目录 {{to}}",
						stopped: "已停止正在运行的文件或项目",
						stopNone: "没有正在运行的文件或项目",
						stopFailed: "停止运行失败",
						runNoTarget: "请选择要运行的文件或项目",
						run: "{{title}} 正在运行",
						runFailed: "运行 {{title}} 失败"
					},
					popup: {
						goToFile: "跳转到文件",
						closingTab: "关闭标签中",
						closingNoSave: "标签未保存，是否确认关闭",
						moving: "移动文件",
						movingInfo: "将移动 {{from}} 到目录 {{to}}",
					},
					upload: {
						text: "点击上传文件夹或拖拽文件到这里进行批量上传。",
						hint: "文件上传会自动开始。",
						selectFile: "选择文件",
						uploading: "上传中",
						startUpload: "开始上传",
						success: "上传成功。",
						failed: "上传失败。"
					},
					action: {
						"ok": "确定",
						"cancel": "取消",
						"confirm": "确认",
					},
					file: {
						new: "新建文件",
						newFolder: "新建文件夹",
						rename: "重命名",
						enterFile: "请输入文件名",
						enterFolder: "请输入文件夹名",
						deleteFile: "是否删除文件 {{name}}",
						deleteFolder: "是否删除文件夹 {{name}}",
						lua: "轻量级、高层级、多范式的编程语言",
						yuescript: "富有表达力且极为简洁的编程语言",
						teal: "静态类型的一种Lua方言编程语言",
						xml: "用标记语言来编写游戏节点树",
						markdown: "来写一些文档吧",
						folder: "创建文件夹",
						yarn: "用于编写游戏对话的语言",
						visualScript: "通过将可视化节点连接在一起来写游戏逻辑",
					},
					editor: {
						goToDefinition: "跳转到定义",
						require: "导入该模块",
					},
					menu: {
						version: "Dorothy SSR 版本号 {{version}}",
						new: "新建",
						delete: "删除",
						rename: "重命名",
						upload: "上传",
						download: "下载",
						save: "保存",
						saveAll: "全部保存",
						close: "关闭",
						closeAll: "全部关闭",
						closeOthers: "关闭其它",
						run: "运行",
						runThis: "运行文件",
						stop: "停止",
						goToFile: "跳转到文件",
					}
				}
			},
		},
		fallbackLng: 'en',
		preload: ['en', 'zh'],
		interpolation: {
			escapeValue: false,
		},
	});

export default i18n;
