import React, { useState } from 'react';
import { styled } from '@mui/material/styles';
import Tabs from '@mui/material/Tabs';
import Tab from '@mui/material/Tab';
import Box from '@mui/material/Box';
import { StyledMenu, StyledMenuItem } from './Menu';
import ListItemIcon from '@mui/material/ListItemIcon';
import ListItemText from '@mui/material/ListItemText';
import {
	AiOutlineClose,
	AiOutlineSave,
	AiFillSave,
	AiFillCloseCircle,
	AiOutlineCloseCircle,
} from 'react-icons/ai';
import { useTranslation } from 'react-i18next';
import { Color } from './Frame';

export type TabStatus = "normal" | "warning" | "error";

interface TabItem {
	key: string;
	title: string;
	contentModified: string | null;
	status: TabStatus;
}

export interface FileTabBarProps {
	index: number | null;
	items: TabItem[];
	onChange: (index: number) => void;
	onMenuClick: (event: TabMenuEvent) => void;
};

interface StyledTabsProps {
	children?: React.ReactNode;
	value: number | false;
	onChange: (event: React.SyntheticEvent, newValue: number) => void;
}

const StyledTabs = styled((props: StyledTabsProps) => (
	<Tabs
		variant='scrollable'
		selectionFollowsFocus
		scrollButtons="auto"
		{...props}
		TabIndicatorProps={{ children: <span className="MuiTabs-indicatorSpan" /> }}
	/>
))({
	'& .MuiTabs-indicator': {
		display: 'flex',
		justifyContent: 'center',
		backgroundColor: 'transparent',
	},
	'& .MuiTabs-indicatorSpan': {
		maxWidth: 40,
		width: '100%',
		backgroundColor: Color.Theme,
	},
});

interface StyledTabProps {
	label: string;
	status: TabStatus;
	onContextMenu: (event: React.MouseEvent) => void;
}

const StyledTab = styled((props: StyledTabProps) => (
	<Tab disableRipple {...props}/>
))(({ theme, status }) => {
	let color = Color.Secondary;
	let selectedColor = Color.Primary;
	switch (status) {
		case "error":
			color = "rgba(233, 133, 116, 0.7)";
			selectedColor = "rgb(233, 133, 116)";
			break;
		case "warning":
			color = "rgba(200, 177, 136, 0.7)";
			selectedColor = "rgb(200, 177, 136)";
			break;
	}
	return {
		textTransform: 'none',
		fontWeight: theme.typography.fontWeightRegular,
		fontSize: theme.typography.pxToRem(15),
		marginRight: theme.spacing(1),
		color,
		'&.Mui-selected': {
			color: selectedColor,
		},
		'&.Mui-focusVisible': {
			backgroundColor: 'rgba(100, 95, 228, 0.32)',
		},
	};
});

export type TabMenuEvent =
	"Save" |
	"SaveAll" |
	"Close" |
	"CloseOthers" |
	"CloseAll";

export default function FileTabBar(props: FileTabBarProps) {
	const {index, items = [], onChange, onMenuClick} = props;
	const [anchorEl, setAnchorEl] = useState<Element | null>(null);
	const [value, setValue] = useState<number | false>(false);
	const {t} = useTranslation();

	if (index !== null && index !== value) {
		setValue(index);
	} else if (index === null && value !== false) {
		setValue(false);
	}

	const handleChange = (_event: React.ChangeEvent<{}>, newValue: number) => {
		setValue(newValue);
		onChange(newValue);
	};

	const onContextMenu = (event: React.MouseEvent) => {
		setAnchorEl(event.currentTarget);
	};

	const handleClose = (event: TabMenuEvent) => {
		setAnchorEl(null);
		onMenuClick(event);
	};

	return (
		<Box width={'100%'}>
			<StyledTabs
				value={value}
				onChange={handleChange}
				aria-label="styled tabs"
			>
				{
					items.map((item) =>
						<StyledTab
							onContextMenu={onContextMenu}
							key={item.key}
							label={item.contentModified !== null ? '*' + item.title : item.title}
							status={item.status}
						/>
					)
				}
			</StyledTabs>
			<StyledMenu
				id="dora-menu"
				anchorEl={anchorEl}
				keepMounted
				autoFocus={false}
				open={Boolean(anchorEl)}
				onClose={handleClose}
			>
				<StyledMenuItem onClick={() => handleClose('Save')}>
					<ListItemIcon>
						<AiOutlineSave/>
					</ListItemIcon>
					<ListItemText primary={ t("menu.save") }/>
					<div style={{fontSize: 10, color: Color.TextSecondary}}>Mod+S</div>
				</StyledMenuItem>
				<StyledMenuItem onClick={() => handleClose('SaveAll')}>
					<ListItemIcon>
						<AiFillSave/>
					</ListItemIcon>
					<ListItemText primary={ t("menu.saveAll") }/>
					<div style={{fontSize: 10, color: Color.TextSecondary}}>Mod+Shift+S</div>
				</StyledMenuItem>
				<StyledMenuItem onClick={() => handleClose('Close')}>
					<ListItemIcon>
						<AiOutlineClose/>
					</ListItemIcon>
					<ListItemText primary={ t("menu.close") }/>
					<div style={{fontSize: 10, color: Color.TextSecondary}}>Mod+W</div>
				</StyledMenuItem>
				<StyledMenuItem onClick={() => handleClose('CloseAll')}>
					<ListItemIcon>
						<AiFillCloseCircle/>
					</ListItemIcon>
					<ListItemText primary={ t("menu.closeAll") }/>
					<div style={{fontSize: 10, color: Color.TextSecondary}}>Mod+Shift+W</div>
				</StyledMenuItem>
				<StyledMenuItem onClick={() => handleClose('CloseOthers')}>
					<ListItemIcon>
						<AiOutlineCloseCircle/>
					</ListItemIcon>
					<ListItemText primary={ t("menu.closeOthers") }/>
				</StyledMenuItem>
			</StyledMenu>
		</Box>
	);
}

